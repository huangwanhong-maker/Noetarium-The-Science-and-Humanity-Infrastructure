// Tourism accounts and levies, as one composition any work may call (ADR 0157): the economics domain's `tourism` on a country's
// symmetric input–output table, visitors' nights and declared spending, and a levy under declared elasticities. Nothing in it names a
// place or a rate.
//
//   tourism.accounts {table: {products, domestic: [[…]], final_domestic, gva, taxes, imported, households_domestic_share},
//                     purchases: [{product, per_night: {origin: spending}, basic_share?}], nights: {origin: nights}, units?}
//                    tourism demand at basic prices, domestic and imported; direct, indirect and total value added; induced imported inputs
//                    and taxes; leakage; the multiplier; the share of the economy's value added; each product's demand, tourism ratio,
//                    direct value added and output induced. `units` is one unit of spending in the table's money (1e-6 when the table
//                    is in millions of the spending's currency)
//
//   tourism.levy     {market: {nights, price, demand_elasticity, supply_elasticity?, displaced?}, levies: [per night], rates?: [on the
//                     operator's price], cap?, value_added_per_night?}
//                    each levy's prices, pass-through, nights after and lost, displaced, revenue, the burdens of visitors and operators,
//                    surpluses lost and the excess burden, and the value added lost with the nights not displaced; the levy in [0, cap]
//                    that raises most

#pragma once

#include <economics/tourism/tourism.hpp>

#include <science/json.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::tourism {

namespace et = economics::tourism;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  for (const auto& x : a.as_array()) v.push_back(x.is_number() ? x.as_number() : 0.0);
  return v;
}

inline et::Accounts accounts_of(const Json& t, double units) {
  et::Accounts A;
  for (const auto& p : t["products"].as_array()) A.domestic.sectors.push_back(p.text());
  const std::size_t n = A.domestic.sectors.size();
  const auto& rows = t["domestic"].as_array();
  if (rows.size() != n) throw std::invalid_argument("tourism: one row of domestic flows per product");
  A.domestic.z = calx::linalg::Matrix(n);
  for (std::size_t i = 0; i < n; i++) {
    const auto r = numbers(rows[i]);
    if (r.size() != n) throw std::invalid_argument("tourism: one column of domestic flows per product");
    for (std::size_t j = 0; j < n; j++) A.domestic.z(i, j) = r[j] / units;
  }
  const auto scale = [&](const char* key) { auto v = numbers(t[key]); for (auto& x : v) x /= units; return v; };
  A.domestic.final_demand = scale("final_domestic");
  A.gva = scale("gva"), A.taxes = scale("taxes"), A.imported = scale("imported");
  A.domestic_share = t["households_domestic_share"].is_array() ? numbers(t["households_domestic_share"]) : std::vector<double>(n, 1.0);
  if (A.domestic.final_demand.size() != n || A.gva.size() != n || A.taxes.size() != n || A.imported.size() != n || A.domestic_share.size() != n)
    throw std::invalid_argument("tourism: one entry per product");
  // A region's table (science/regional.hpp, ADR 0159) carries its purchases from the rest of the country and households' share of them.
  if (t["rest"].is_array()) A.rest = scale("rest");
  if (t["households_rest_share"].is_array()) A.rest_share = numbers(t["households_rest_share"]);
  // Persons employed a year per unit of the table's money (ADR 0161), carried to the spending's money.
  if (t["jobs"].is_array()) {
    A.jobs = numbers(t["jobs"]);
    for (auto& j : A.jobs) j *= units;
  }
  if ((!A.rest.empty() && A.rest.size() != n) || (!A.rest_share.empty() && A.rest_share.size() != n) || (!A.jobs.empty() && A.jobs.size() != n))
    throw std::invalid_argument("tourism: one entry per product");
  // Products with no output (households as employers, extraterritorial bodies, where a country has none) have no flows; they are left
  // out, which changes nothing else.
  const auto x0 = A.domestic.output();
  std::vector<std::size_t> keep;
  for (std::size_t i = 0; i < n; i++)
    if (x0[i] > 0) keep.push_back(i);
  if (keep.size() < n) {
    et::Accounts B;
    const std::size_t m = keep.size();
    B.domestic.z = calx::linalg::Matrix(m);
    for (std::size_t a = 0; a < m; a++) {
      B.domestic.sectors.push_back(A.domestic.sectors[keep[a]]);
      B.domestic.final_demand.push_back(A.domestic.final_demand[keep[a]]);
      B.gva.push_back(A.gva[keep[a]]), B.taxes.push_back(A.taxes[keep[a]]), B.imported.push_back(A.imported[keep[a]]), B.domestic_share.push_back(A.domestic_share[keep[a]]);
      if (!A.rest.empty()) B.rest.push_back(A.rest[keep[a]]);
      if (!A.rest_share.empty()) B.rest_share.push_back(A.rest_share[keep[a]]);
      if (!A.jobs.empty()) B.jobs.push_back(A.jobs[keep[a]]);
      for (std::size_t b = 0; b < m; b++) B.domestic.z(a, b) = A.domestic.z(keep[a], keep[b]);
    }
    A = std::move(B);
  }
  const std::size_t k = A.domestic.n();
  const auto x = A.domestic.output();
  A.domestic.value_added.resize(k);
  for (std::size_t j = 0; j < k; j++) {
    double col = 0;
    for (std::size_t i = 0; i < k; i++) col += A.domestic.z(i, j);
    A.domestic.value_added[j] = x[j] - col;  // the rest of each column, so that the table balances by rows
  }
  return A;
}

inline Json accounts(const Json& r) {
  const double units = num(r["units"], 1.0);
  const auto A = accounts_of(r["table"], units);
  std::vector<et::Purchase> purchases;
  for (const auto& p : r["purchases"].as_array()) {
    et::Purchase q{p["product"].text(), {}, num(p["basic_share"], 1.0)};
    for (const auto& [o, v] : p["per_night"].as_object()) q.per_night[o] = v.as_number();
    purchases.push_back(q);
  }
  std::map<std::string, double> nights;
  for (const auto& [o, v] : r["nights"].as_object()) nights[o] = v.as_number();
  std::vector<double> d;
  double abroad = 0;
  const auto& tw = r["table"]["two_region"];
  if (tw.is_object()) {
    // A two-region table (science/regional.hpp, ADR 0160): a purchase made in the region is split among the region (its local share),
    // the rest of the country (the rest of the national domestic share) and abroad.
    std::map<std::string, std::size_t> at;
    for (std::size_t i = 0; i < A.domestic.n(); i++) at[A.domestic.sectors[i]] = i;
    std::map<std::string, std::pair<double, double>> share;
    const auto& base = tw["base"].as_array();
    for (std::size_t k = 0; k < base.size(); k++) share[base[k].text()] = {tw["national_share"][k].as_number(), tw["local_share"][k].as_number()};
    d.assign(A.domestic.n(), 0.0);
    for (const auto& p : purchases) {
      const auto f = share.find(p.product);
      if (f == share.end()) throw std::invalid_argument("tourism: no product " + p.product + " in the table");
      double amount = 0;
      for (const auto& [o, v] : p.per_night) amount += nights.at(o) * v * p.basic_share;
      const auto put = [&](const std::string& code, double v) {
        if (!(v > 0)) return;
        const auto it = at.find(code);
        if (it == at.end()) throw std::invalid_argument("tourism: no product " + code + " with output in the table");
        d[it->second] += v;
      };
      put("r:" + p.product, amount * f->second.second);
      put("s:" + p.product, amount * (f->second.first - f->second.second));
      abroad += amount * (1 - f->second.first);
    }
  } else {
    d = et::demand(A, purchases, nights);
  }
  const auto E = et::effects(A, d);
  double spending = 0, all_nights = 0;
  for (const auto& p : purchases)
    for (const auto& [o, v] : p.per_night) spending += v * nights.at(o);
  for (const auto& [o, v] : nights) all_nights += v;
  Json products = Json::array();
  const auto M = economics::leontief(A.domestic);
  for (std::size_t i = 0; i < A.domestic.n(); i++) {
    if (!(d[i] > 0) && !(E.total_output[i] > 0)) continue;
    products.push(Json::object({{"product", A.domestic.sectors[i]}, {"demand", d[i]}, {"demand_domestic", d[i] * A.domestic_share[i]}, {"ratio", E.ratio[i]}, {"direct_gva", A.gva[i] / M.output[i] * d[i] * A.domestic_share[i]},
                                {"output_induced", E.total_output[i]}, {"gva_induced", A.gva[i] / M.output[i] * E.total_output[i]}}));
  }
  if (tw.is_object()) {
    // By region: value added directly and along the chains in the region and in the rest of the country; the purchases abroad.
    double gr = 0, gs = 0, dr = 0, ds = 0, jr = 0, js = 0;
    for (std::size_t i = 0; i < A.domestic.n(); i++) {
      const double c = A.gva[i] / M.output[i];
      const bool region = A.domestic.sectors[i].rfind("r:", 0) == 0;
      (region ? gr : gs) += c * E.total_output[i];
      (region ? dr : ds) += c * d[i];
      if (!A.jobs.empty()) (region ? jr : js) += A.jobs[i] * E.total_output[i];
    }
    const double all = E.demand + abroad;
    return Json::object({{"spending", spending}, {"nights", all_nights}, {"demand", all}, {"demand_domestic", E.demand_domestic}, {"demand_imported", abroad},
                         {"direct_gva", E.direct_gva}, {"indirect_gva", E.indirect_gva}, {"total_gva", E.total_gva}, {"induced_imports", E.induced_imports},
                         {"induced_taxes", E.induced_taxes}, {"demand_rest", 0.0}, {"induced_rest", 0.0}, {"leakage_country", 0.0}, {"unattributed", E.unattributed},
                         {"gva_multiplier", E.gva_multiplier}, {"leakage", all > 0 ? (abroad + E.induced_imports) / all : 0.0}, {"direct_share_of_gva", E.direct_share_of_gva},
                         {"value_added_per_night", all_nights > 0 ? E.total_gva / all_nights : 0.0}, {"table_imbalance", A.imbalance()}, {"products", products},
                         {"gva_region", gr}, {"gva_rest", gs}, {"direct_gva_region", dr}, {"direct_gva_rest", ds},
                         {"jobs_direct", E.jobs_direct}, {"jobs_total", E.jobs_total}, {"jobs_region", jr}, {"jobs_rest", js}});
  }
  return Json::object({{"spending", spending}, {"nights", all_nights}, {"demand", E.demand}, {"demand_domestic", E.demand_domestic}, {"demand_imported", E.demand_imported},
                       {"direct_gva", E.direct_gva}, {"indirect_gva", E.indirect_gva}, {"total_gva", E.total_gva}, {"induced_imports", E.induced_imports},
                       {"induced_taxes", E.induced_taxes}, {"demand_rest", E.demand_rest}, {"induced_rest", E.induced_rest}, {"leakage_country", E.leakage_country},
                       {"unattributed", E.unattributed}, {"gva_multiplier", E.gva_multiplier}, {"leakage", E.leakage},
                       {"direct_share_of_gva", E.direct_share_of_gva}, {"value_added_per_night", all_nights > 0 ? E.total_gva / all_nights : 0.0},
                       {"table_imbalance", A.imbalance()}, {"products", products}, {"jobs_direct", E.jobs_direct}, {"jobs_total", E.jobs_total}});
}

inline Json levy_json(const et::Levy& L, double va) {
  return Json::object({{"levy", L.levy}, {"consumer_price", L.consumer_price}, {"producer_price", L.producer_price}, {"pass_through", L.pass_through},
                       {"nights_after", L.nights_after}, {"nights_lost", L.nights_lost}, {"displaced_nights", L.displaced_nights}, {"revenue", L.revenue},
                       {"visitors_burden", L.visitors_burden}, {"operators_burden", L.operators_burden}, {"consumer_surplus_loss", L.consumer_surplus_loss},
                       {"producer_surplus_loss", L.producer_surplus_loss}, {"excess_burden", L.excess_burden},
                       {"value_added_lost", finite(va * (L.nights_lost - L.displaced_nights))}});
}

inline Json levy(const Json& r) {
  const auto& m = r["market"];
  et::Market M{num(m["nights"], 0), num(m["price"], 0), num(m["demand_elasticity"], -1), num(m["supply_elasticity"], et::infinite), num(m["displaced"], 0)};
  const double va = num(r["value_added_per_night"], std::nan(""));
  Json flat = Json::array(), rates = Json::array();
  if (r["levies"].is_array())
    for (const auto& t : r["levies"].as_array()) flat.push(levy_json(et::flat(M, t.as_number()), va));
  if (r["rates"].is_array())
    for (const auto& x : r["rates"].as_array()) {
      Json j = levy_json(et::ad_valorem(M, x.as_number()), va);
      j.set("rate", x.as_number());
      rates.push(j);
    }
  Json peak = Json();
  if (r["cap"].is_number()) {
    const auto P = et::revenue_maximising(M, r["cap"].as_number());
    peak = Json::object({{"levy", P.levy}, {"revenue", P.revenue}, {"at_cap", P.at_cap}});
  }
  return Json::object({{"flat", flat}, {"rates", rates}, {"peak", peak},
                       {"small_levy_pass_through", std::isinf(M.supply_elasticity) ? 1.0 : M.supply_elasticity / (M.supply_elasticity - M.demand_elasticity)}});
}

inline bool handles(const std::string& op) { return op.rfind("tourism.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "tourism.accounts") return accounts(r);
  if (op == "tourism.levy") return levy(r);
  throw std::invalid_argument("tourism: no operation " + op);
}

}  // namespace science::tourism
