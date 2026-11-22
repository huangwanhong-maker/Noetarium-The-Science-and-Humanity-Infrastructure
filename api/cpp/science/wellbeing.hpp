// Household well-being losses as one composition any work may call (ADR 0150): risk management's `wellbeing` on declared households
// and events, with measures compared by the asset losses and the well-being losses they avoid. Nothing in it names a place.
//
//   wellbeing.assess   {households: [{id, group, persons, income, transfers?, savings?, private_share?, vulnerability, warned?}],
//                      events: [{return_period, exposed: [share for each household]}], readings?: {eta, rho, productivity, rebuild_years,
//                      horizon, early_warning, floor, hard_minimum, slowest_rebuild_years, optimal_rebuilding}, support?: {share,
//                      reference_group, exclusion, inclusion}, poverty_line?, measures?: [{name, kind, group?, value?, support?}]}
//                      the expected asset loss, equivalent consumption loss and resilience, by group and household, each event's, and
//                      for each measure what it avoids of each and its rank by each
//
// A measure changes the declarations and the whole is computed again:
//   faster    rebuild_years = value                       stronger  vulnerability × (1 − value), of a group or all
//   warning   warned = true, of a group or all            savings   savings + value a person, of a group or all
//   protect   exposed × (1 − value), of a group or all    support   support = the measure's own {share, reference_group, …}

#pragma once

#include <risk_management/wellbeing/wellbeing.hpp>

#include <science/json.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::wellbeing {

namespace rw = risk_management::wellbeing;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline rw::Readings readings_of(const Json& r) {
  rw::Readings R;
  if (!r.is_object()) return R;
  R.eta = num(r["eta"], R.eta);
  R.rho = num(r["rho"], R.rho);
  R.productivity = num(r["productivity"], R.productivity);
  R.rebuild_years = num(r["rebuild_years"], R.rebuild_years);
  R.horizon = num(r["horizon"], R.horizon);
  R.early_warning = num(r["early_warning"], R.early_warning);
  R.floor = num(r["floor"], R.floor);
  R.hard_minimum = num(r["hard_minimum"], R.hard_minimum);
  R.slowest_rebuild_years = num(r["slowest_rebuild_years"], R.slowest_rebuild_years);
  R.optimal_rebuilding = r["optimal_rebuilding"].is_bool() && r["optimal_rebuilding"].as_bool();
  R.check();
  return R;
}

inline rw::Support support_of(const Json& s) {
  rw::Support S;
  if (!s.is_object()) return S;
  S.share = num(s["share"], 0);
  S.reference_group = s["reference_group"].text("");
  S.exclusion = num(s["exclusion"], 0);
  S.inclusion = num(s["inclusion"], 0);
  return S;
}

inline std::vector<rw::Household> households_of(const Json& a) {
  std::vector<rw::Household> H;
  std::size_t k = 0;
  for (const auto& x : a.as_array()) {
    rw::Household h;
    h.id = x["id"].text("h" + std::to_string(++k));
    h.group = x["group"].text("");
    h.weight = num(x["persons"], 1);
    h.income = x["income"].as_number();
    h.transfers = num(x["transfers"], 0);
    h.savings = num(x["savings"], 0);
    h.private_share = num(x["private_share"], 1);
    h.vulnerability = x["vulnerability"].as_number();
    h.warned = x["warned"].is_bool() && x["warned"].as_bool();
    H.push_back(h);
  }
  rw::check(H);
  return H;
}

inline std::vector<rw::Event> events_of(const Json& a, std::size_t n) {
  std::vector<rw::Event> E;
  for (const auto& e : a.as_array()) {
    rw::Event ev{e["return_period"].as_number(), {}};
    for (const auto& x : e["exposed"].as_array()) ev.exposed.push_back(x.as_number());
    if (ev.exposed.size() != n) throw std::invalid_argument("wellbeing: an event gives an exposed share for every household");
    E.push_back(std::move(ev));
  }
  if (E.empty()) throw std::invalid_argument("wellbeing: at least one event");
  return E;
}

inline Json summary(const rw::Annual& A, const std::vector<rw::Household>& H) {
  Json groups = Json::array();
  for (const auto& [name, g] : A.by_group)
    groups.push(Json::object({{"group", name}, {"persons", g.persons}, {"affected", g.affected}, {"asset_loss", g.asset_loss}, {"equivalent", g.equivalent},
                              {"asset_share", A.asset_loss > 0 ? g.asset_loss / A.asset_loss : 0.0}, {"equivalent_share", A.equivalent > 0 ? g.equivalent / A.equivalent : 0.0},
                              {"resilience", g.equivalent > 0 ? g.asset_loss / g.equivalent : 0.0}, {"pushed_below", g.pushed_below}}));
  Json events = Json::array();
  for (std::size_t j = 0; j < A.events.size(); j++) {
    const auto& o = A.events[j];
    events.push(Json::object({{"return_period", o.return_period}, {"weight", A.weights[j]}, {"asset_loss", o.asset_loss}, {"equivalent", o.equivalent},
                              {"resilience", o.resilience}, {"income_loss", o.income_loss}, {"support_paid", o.support_paid}, {"pushed_below", o.pushed_below},
                              {"capped", static_cast<double>(o.capped)}, {"slowed", static_cast<double>(o.slowed)}}));
  }
  Json households = Json::array();
  for (std::size_t h = 0; h < H.size(); h++)
    households.push(Json::object({{"id", H[h].id}, {"group", H[h].group}, {"asset_loss", A.household_asset_loss[h]}, {"equivalent", A.household_equivalent[h]}}));
  return Json::object({{"asset_loss", A.asset_loss}, {"equivalent", A.equivalent}, {"resilience", A.resilience}, {"pushed_below", A.pushed_below},
                       {"groups", groups}, {"events", events}, {"households", households}});
}

inline Json assess(const Json& r) {
  const auto R0 = readings_of(r["readings"]);
  const auto H0 = households_of(r["households"]);
  const auto E0 = events_of(r["events"], H0.size());
  const auto S0 = support_of(r["support"]);
  rw::Options O;
  O.poverty_line = num(r["poverty_line"], 0);
  const auto base = rw::annual(H0, R0, E0, S0, O);
  Json out = Json::object({{"baseline", summary(base, H0)}, {"lambda", R0.lambda()}, {"mean_consumption", rw::calibrate(H0, R0).mean_consumption}});
  if (!r["measures"].is_array()) return out;
  struct Row {
    std::string name, kind;
    double assets = 0, equivalent = 0;
  };
  std::vector<Row> rows;
  for (const auto& m : r["measures"].as_array()) {
    auto H = H0;
    auto E = E0;
    auto R = R0;
    auto S = S0;
    const std::string kind = m["kind"].text(), group = m["group"].text("");
    const double v = num(m["value"], 0);
    const auto in = [&](const rw::Household& h) { return group.empty() || h.group == group; };
    if (kind == "faster") R.rebuild_years = v, R.check();
    else if (kind == "stronger") {
      if (!(v >= 0 && v <= 1)) throw std::invalid_argument("wellbeing: a measure making buildings stronger removes a share in [0, 1] of vulnerability");
      for (auto& h : H) if (in(h)) h.vulnerability *= 1 - v;
    } else if (kind == "warning") {
      for (auto& h : H) if (in(h)) h.warned = true;
    } else if (kind == "savings") {
      for (auto& h : H) if (in(h)) h.savings += v;
    } else if (kind == "protect") {
      if (!(v >= 0 && v <= 1)) throw std::invalid_argument("wellbeing: protection removes a share in [0, 1] of exposure");
      for (auto& e : E)
        for (std::size_t h = 0; h < H.size(); h++) if (in(H[h])) e.exposed[h] *= 1 - v;
    } else if (kind == "support") {
      S = support_of(m["support"]);
    } else {
      throw std::invalid_argument("wellbeing: a measure is faster, stronger, warning, savings, protect or support, not " + kind);
    }
    rw::check(H);
    const auto A = rw::annual(H, R, E, S, O);
    rows.push_back({m["name"].text(kind), kind, base.asset_loss - A.asset_loss, base.equivalent - A.equivalent});
  }
  std::vector<std::size_t> by_assets(rows.size()), by_wellbeing(rows.size());
  std::iota(by_assets.begin(), by_assets.end(), 0);
  std::iota(by_wellbeing.begin(), by_wellbeing.end(), 0);
  std::stable_sort(by_assets.begin(), by_assets.end(), [&](std::size_t a, std::size_t b) { return rows[a].assets > rows[b].assets; });
  std::stable_sort(by_wellbeing.begin(), by_wellbeing.end(), [&](std::size_t a, std::size_t b) { return rows[a].equivalent > rows[b].equivalent; });
  std::vector<double> rank_a(rows.size()), rank_w(rows.size());
  for (std::size_t k = 0; k < rows.size(); k++) {
    rank_a[by_assets[k]] = static_cast<double>(k + 1);
    rank_w[by_wellbeing[k]] = static_cast<double>(k + 1);
  }
  Json ms = Json::array();
  for (std::size_t k = 0; k < rows.size(); k++)
    ms.push(Json::object({{"name", rows[k].name}, {"kind", rows[k].kind}, {"avoided_asset_loss", rows[k].assets}, {"avoided_equivalent", rows[k].equivalent},
                          {"rank_by_assets", rank_a[k]}, {"rank_by_wellbeing", rank_w[k]}}));
  out.set("measures", ms);
  return out;
}

inline bool handles(const std::string& op) { return op.rfind("wellbeing.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "wellbeing.assess") return assess(r);
  throw std::invalid_argument("wellbeing: no operation " + op);
}

}  // namespace science::wellbeing
