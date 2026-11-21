// Land value capture and land readjustment as one composition any work may call (ADR 0146): the economics domain's `capture`, read and
// answered as JSON. Every rate, share, cap and value is the requester's declaration.
//
//   capture.compare      {parcels, rate, cost: [{year, amount}], instruments: {levy, rights, auction, increment, obligation}, statutory}
//                        each declared instrument's payments, its present value, cost recovery ρ, capture of the counterfactual uplift κ
//                        beside the statutory rate σ, who pays; what tax increment diverts from each jurisdiction; the parcels whose
//                        uplift is taken more than once over
//   capture.readjust     {areas: {public_before, private_before, public_after, returned, reserve}, value_before, value_after, cap,
//                        selling_cost, subsidies, costs, sale_year, rate, owners, plots}
//                        the area identity, the contribution ratio with its parts, whether the owners' land keeps its value, the cap,
//                        the reserve land's revenue and the project's balance; and, with owners and plots, the replotting by the least
//                        total distance with each owner's entitlement and settlement
//
// parcels [{id, area, without, with, frontage, x, y}]; levy {cost, markup, share, basis: uplift | area | frontage, first, instalments};
// rights {added_floor, basic_far, land_price, factor, year, parcel}; auction {authorised, minimum, bids, year}; increment {assessed: [AV_1 …],
// base, share, rates: {jurisdiction: rate}, but_for}; obligation {units, rate, to_land, to_buyers, year}; plots [{id, area,
// unit_value, x, y}].
#pragma once

#include <economics/capture/capture.hpp>

#include <science/json.hpp>
#include <science/scorecards.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::capture {

namespace ec = economics::capture;
using science::scorecards::list;

inline double num(const Json& j, double fallback = ec::none) { return j.is_number() ? j.as_number() : fallback; }
inline Json maybe(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline std::vector<ec::Parcel> parcels_of(const Json& j) {
  std::vector<ec::Parcel> out;
  for (const auto& p : list(j))
    out.push_back({p["id"].text(), num(p["area"], 0), num(p["without"], 0), num(p["with"], 0), num(p["frontage"], 0), num(p["x"], 0), num(p["y"], 0)});
  return out;
}

inline std::vector<ec::Payment> payments_of(const Json& j, const std::string& payer) {
  std::vector<ec::Payment> out;
  for (const auto& p : list(j)) out.push_back({static_cast<int>(num(p["year"], 0)), num(p["amount"], 0), p["payer"].text(payer), p["parcel"].text(), false});
  return out;
}

inline Json payments_json(const std::vector<ec::Payment>& ps) {
  Json a = Json::array();
  for (const auto& p : ps) a.push(Json::object({{"year", static_cast<double>(p.year)}, {"amount", p.amount}, {"payer", p.payer}, {"parcel", p.parcel}, {"from_uplift", p.from_uplift}}));
  return a;
}

inline Json comparison_json(const ec::Comparison& c) {
  Json by = Json::object();
  for (const auto& [k, v] : c.by_payer) by.set(k, v);
  return Json::object({{"instrument", c.instrument}, {"pv", c.pv}, {"cost_recovery", maybe(c.cost_recovery)}, {"capture", maybe(c.capture)}, {"statutory", maybe(c.statutory)},
                       {"by_payer", by}});
}

inline Json compare_op(const Json& q) {
  const auto ps = parcels_of(q["parcels"]);
  if (ps.empty()) throw std::invalid_argument("capture: the parcels the project touches");
  const double rate = num(q["rate"], 0.04);
  const double pv_cost = ec::present_value(payments_of(q["cost"], "the project"), rate);
  double pv_uplift = 0;
  for (const auto& p : ps) pv_uplift += std::max(0.0, p.uplift());  // values at the valuation date
  const Json& ins = q["instruments"];
  const Json& stat = q["statutory"];
  Json rows = Json::array(), detail = Json::object();
  std::vector<std::vector<ec::Payment>> all;
  auto add = [&](const std::string& name, const std::vector<ec::Payment>& pays) {
    all.push_back(pays);
    rows.push(comparison_json(ec::compare(name, pays, rate, pv_cost, pv_uplift, num(stat[name]))));
    detail.set(name, payments_json(pays));
  };
  if (ins.has("levy") && !ins["levy"].is_null()) {
    const Json& l = ins["levy"];
    const std::string b = l["basis"].text("uplift");
    add("levy", ec::levy(ps, num(l["cost"], 0), num(l["markup"], 0), num(l["share"], 0), b == "area" ? ec::Basis::area : b == "frontage" ? ec::Basis::frontage : ec::Basis::uplift,
                         static_cast<int>(num(l["first"], 0)), static_cast<int>(num(l["instalments"], 1))));
  }
  if (ins.has("rights") && !ins["rights"].is_null()) {
    const Json& r = ins["rights"];
    auto pay = ec::rights_by_formula(num(r["added_floor"], 0), num(r["basic_far"], 1), num(r["land_price"], 0), num(r["factor"], 1), static_cast<int>(num(r["year"], 0)));
    pay.parcel = r["parcel"].text();  // the parcel the added floor is granted on, if named, so that a charge twice over is found
    add("rights", {pay});
  }
  Json auction;
  if (ins.has("auction") && !ins["auction"].is_null()) {
    const Json& a = ins["auction"];
    std::vector<double> bids;
    for (const auto& b : list(a["bids"])) bids.push_back(b.as_number());
    const auto r = ec::rights_by_auction(static_cast<std::size_t>(num(a["authorised"], 0)), num(a["minimum"], 0), bids);
    auction = Json::object({{"authorised", static_cast<double>(r.authorised)}, {"sold", static_cast<double>(r.sold)}, {"unsold", static_cast<double>(r.unsold)}, {"price", r.price},
                            {"revenue", r.revenue}});
    add("auction", {{static_cast<int>(num(a["year"], 0)), r.revenue, "developers", "", true}});
  }
  Json increment;
  if (ins.has("increment") && !ins["increment"].is_null()) {
    const Json& t = ins["increment"];
    std::vector<double> av;
    for (const auto& v : list(t["assessed"])) av.push_back(v.as_number());
    std::map<std::string, double> rates;
    if (t["rates"].is_object())
      for (const auto& [k, v] : t["rates"].as_object()) rates[k] = v.as_number();
    const auto inc = ec::tax_increment(av, num(t["base"], 0), num(t["share"], 1), rates, num(t["but_for"], 1), rate);
    Json by = Json::object();
    std::map<std::string, double> diverted;
    for (const auto& p : inc.diverted) diverted[p.payer] += p.amount / std::pow(1 + rate, p.year);
    for (const auto& [k, v] : diverted) by.set(k, v);
    increment = Json::object({{"pv_district", inc.pv_district}, {"pv_diverted", inc.pv_diverted}, {"diverted_by_jurisdiction", by}});
    // Only the project's share of the increment is taken from the uplift; the rest would have come anyway.
    const double bf = num(t["but_for"], 1);
    std::vector<ec::Payment> split;
    for (const auto& p : inc.district) split.push_back({p.year, p.amount * bf, p.payer, "", true}), split.push_back({p.year, p.amount * (1 - bf), p.payer, "", false});
    add("increment", split);
  }
  Json obligation;
  if (ins.has("obligation") && !ins["obligation"].is_null()) {
    const Json& o = ins["obligation"];
    const auto ob = ec::developer_obligation(num(o["units"], 0), num(o["rate"], 0), num(o["to_land"], 1), num(o["to_buyers"], 0), static_cast<int>(num(o["year"], 0)));
    obligation = Json::object({{"on_land", ob.on_land}, {"on_buyers", ob.on_buyers}});
    add("obligation", ob.payments);
  }
  Json doubles = Json::array();
  for (const auto& d : ec::double_charges(ps, all, rate)) doubles.push(Json::object({{"parcel", d.parcel}, {"uplift", d.uplift}, {"taken", d.taken}}));
  return Json::object({{"pv_cost", pv_cost}, {"pv_uplift", pv_uplift}, {"rate", rate}, {"instruments", rows}, {"payments", detail}, {"auction", auction}, {"increment", increment},
                       {"obligation", obligation}, {"double_charges", doubles}});
}

inline Json readjust_op(const Json& q) {
  const Json& m = q["areas"];
  const ec::Areas a{num(m["public_before"], 0), num(m["private_before"], 0), num(m["public_after"], 0), num(m["returned"], 0), num(m["reserve"], 0)};
  std::vector<ec::Payment> subsidies = payments_of(q["subsidies"], "subsidy");
  for (auto& p : subsidies) p.from_uplift = false;
  const auto r = ec::readjust(a, num(q["value_before"]), num(q["value_after"]), num(q["cap"]), num(q["selling_cost"], 0), subsidies, payments_of(q["costs"], "the project"),
                              static_cast<int>(num(q["sale_year"], 0)), num(q["rate"], 0.04));
  Json out = Json::object({{"areas_balance", r.areas_balance}, {"imbalance", r.imbalance}, {"contribution", r.contribution}, {"public_share", r.public_share},
                           {"reserve_share", r.reserve_share}, {"uplift_ratio", r.uplift_ratio}, {"kept", r.kept}, {"value_kept", r.value_kept}, {"within_cap", r.within_cap},
                           {"reserve_revenue", r.reserve_revenue}, {"pv_balance", r.pv_balance}, {"feasible", r.feasible}});
  if (q.has("owners") && q.has("plots")) {
    const auto owners = parcels_of(q["owners"]);
    std::vector<ec::Plot> plots;
    for (const auto& p : list(q["plots"])) plots.push_back({p["id"].text(), num(p["area"], 0), num(p["unit_value"], 0), num(p["x"], 0), num(p["y"], 0)});
    const auto rp = ec::replot(owners, plots);
    Json rows = Json::array();
    for (std::size_t i = 0; i < owners.size(); i++)
      rows.push(Json::object({{"owner", owners[i].id}, {"plot", plots[rp.plot_of[i]].id}, {"entitlement", rp.entitlement[i]}, {"received", rp.received[i]}, {"settlement", rp.settlement[i]}}));
    out.set("replotting", Json::object({{"assignments", rows}, {"distance", rp.distance}}));
  }
  return out;
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "capture.compare") return compare_op(q);
  if (op == "capture.readjust") return readjust_op(q);
  throw std::invalid_argument("capture: no operation \"" + op + "\"");
}

}  // namespace science::capture
