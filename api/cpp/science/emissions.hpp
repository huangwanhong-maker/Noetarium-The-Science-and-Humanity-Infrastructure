// A community's greenhouse gas inventory and its mitigation as one composition any work may call (ADR 0147): the industrial-ecology
// domain's `emissions`, read and answered as JSON. Every factor, potential, rate, cost and target is the requester's declaration.
//
//   emissions.inventory  {inventory}                          the totals by level (BASIC, BASIC+, territorial) with their uncertainty
//                                                             by the IPCC's Approach 1, biogenic CO2 apart, every (sector, scope) cell,
//                                                             the gases, completeness with its gaps, intensities, key categories
//   emissions.curve      {inventory, measures, level, order}  each measure alone, in the curve's order and by its Shapley value, with
//                                                             costs per tonne; the joint abatement and the overlap counted once
//   emissions.pathway    {inventory, measures, growth, last, target: {year, reduction, budget}, level}
//                                                             the years ahead without and with the measures, their cost, the gap to
//                                                             the target and the cumulative against the budget
//
// inventory {name, boundary, year, population, gdp, potentials: "AR5" | "AR6" | {name, source, gwp: {gas: kg CO2e per kg}},
// entries: [{id, sector, scope, subsector, gas, activity, unit, factor, amount, u_activity, u_factor, key, included_in,
// grid_supply, imported_waste, biogenic}]}; sector one of stationary energy, transportation, waste, ippu, afolu, other; key one of
// NO, IE, NE, C (absent for an estimate). measures [{id, name, sector (empty or "any": every sector), scope, subsector, gas, on: activity | factor, reduction,
// cost, start, ramp}]. growth {sector: rate}. level basic | basic+ | territorial.
#pragma once

#include <industrial_ecology/emissions/emissions.hpp>

#include <science/json.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::emissions {

namespace ie = industrial_ecology::emissions;

inline const Json::Array& items(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}
inline double num(const Json& j, double fallback = ie::none) { return j.is_number() ? j.as_number() : fallback; }
inline Json maybe(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline ie::Sector sector_of(const std::string& s) {
  if (s == "stationary energy" || s == "stationary_energy" || s == "I") return ie::Sector::stationary_energy;
  if (s == "transportation" || s == "II") return ie::Sector::transportation;
  if (s == "waste" || s == "III") return ie::Sector::waste;
  if (s == "ippu" || s == "industrial processes and product use" || s == "IV") return ie::Sector::ippu;
  if (s == "afolu" || s == "agriculture, forestry and other land use" || s == "V") return ie::Sector::afolu;
  if (s == "other" || s == "other scope 3" || s == "VI") return ie::Sector::other;
  throw std::invalid_argument("emissions: no sector \"" + s + "\" (stationary energy, transportation, waste, ippu, afolu, other)");
}
inline std::string sector_word(ie::Sector s) { return ie::name(s); }

inline ie::Key key_of(const std::string& k) {
  if (k.empty()) return ie::Key::estimated;
  if (k == "NO") return ie::Key::NO;
  if (k == "IE") return ie::Key::IE;
  if (k == "NE") return ie::Key::NE;
  if (k == "C") return ie::Key::C;
  throw std::invalid_argument("emissions: no notation key \"" + k + "\" (NO, IE, NE, C)");
}

inline ie::Level level_of(const std::string& l) {
  if (l.empty() || l == "basic+" || l == "basic_plus" || l == "BASIC+") return ie::Level::basic_plus;
  if (l == "basic" || l == "BASIC") return ie::Level::basic;
  if (l == "territorial") return ie::Level::territorial;
  throw std::invalid_argument("emissions: no level \"" + l + "\" (basic, basic+, territorial)");
}

inline ie::Potentials potentials_of(const Json& j) {
  if (j.is_null() || j.text() == "AR5") return ie::ar5();
  if (j.text() == "AR6") return ie::ar6();
  if (!j.is_object()) throw std::invalid_argument("emissions: potentials are \"AR5\", \"AR6\" or {name, source, gwp}");
  ie::Potentials p{j["name"].text("declared"), j["source"].text(), {}};
  if (j["gwp"].is_object())
    for (const auto& [g, v] : j["gwp"].as_object()) p.gwp[g] = v.as_number();
  p.gwp.emplace("CO2e", 1.0);
  return p;
}

inline ie::Inventory inventory_of(const Json& j) {
  ie::Inventory I;
  I.name = j["name"].text(), I.boundary = j["boundary"].text(), I.year = static_cast<int>(num(j["year"], 0));
  I.population = num(j["population"]), I.gdp = num(j["gdp"]);
  I.potentials = potentials_of(j["potentials"]);
  for (const auto& e : items(j["entries"])) {
    ie::Entry x;
    x.id = e["id"].text(), x.sector = sector_of(e["sector"].text()), x.scope = static_cast<int>(num(e["scope"], 1));
    x.subsector = e["subsector"].text(), x.gas = e["gas"].text("CO2"), x.unit = e["unit"].text();
    x.activity = num(e["activity"]), x.factor = num(e["factor"]), x.amount = num(e["amount"]);
    x.u_activity = num(e["u_activity"], 0), x.u_factor = num(e["u_factor"], 0);
    x.key = key_of(e["key"].text()), x.included_in = e["included_in"].text();
    x.grid_supply = e["grid_supply"].is_bool() && e["grid_supply"].as_bool();
    x.imported_waste = e["imported_waste"].is_bool() && e["imported_waste"].as_bool();
    x.biogenic = e["biogenic"].is_bool() && e["biogenic"].as_bool();
    I.entries.push_back(x);
  }
  if (I.entries.empty()) throw std::invalid_argument("emissions: an inventory of entries");
  return I;
}

inline std::vector<ie::Measure> measures_of(const Json& j) {
  std::vector<ie::Measure> out;
  for (const auto& m : items(j)) {
    ie::Measure x;
    x.id = m["id"].text(), x.name = m["name"].text(x.id);
    const std::string sec = m["sector"].text();
    if (sec.empty() || sec == "any") x.any_sector = true;
    else x.sector = sector_of(sec);
    x.scope = static_cast<int>(num(m["scope"], 0)), x.subsector = m["subsector"].text(), x.gas = m["gas"].text();
    const std::string on = m["on"].text("activity");
    if (on != "activity" && on != "factor") throw std::invalid_argument("emissions: measure " + x.id + " acts on an activity or a factor");
    x.on = on == "factor" ? ie::On::factor : ie::On::activity;
    x.reduction = num(m["reduction"], 0), x.cost = num(m["cost"], 0);
    x.start = static_cast<int>(num(m["start"], 0)), x.ramp = static_cast<int>(num(m["ramp"], 1));
    out.push_back(x);
  }
  return out;
}

inline Json inventory_op(const Json& q) {
  const auto I = inventory_of(q["inventory"]);
  const auto T = ie::totals(I);
  Json cells = Json::array();
  for (const auto& c : T.cells)
    cells.push(Json::object({{"sector", sector_word(c.sector)}, {"scope", static_cast<double>(c.scope)}, {"tonnes", c.tonnes}, {"estimated", static_cast<double>(c.estimated)},
                             {"keyed", static_cast<double>(c.keyed)}, {"missing", c.missing}, {"not_estimated", c.not_estimated}}));
  Json gases = Json::object();
  for (const auto& [g, t] : T.by_gas) gases.set(g, t);
  auto strings = [](const std::vector<std::string>& v) {
    Json a = Json::array();
    for (const auto& s : v) a.push(s);
    return a;
  };
  Json keys = Json::array();
  for (const auto& c : ie::key_categories(I, level_of(q["level"].text("basic+"))))
    keys.push(Json::object({{"sector", c.sector}, {"scope", static_cast<double>(c.scope)}, {"subsector", c.subsector}, {"gas", c.gas}, {"tonnes", c.tonnes}, {"share", c.share},
                            {"cumulative", c.cumulative}, {"key", c.key}}));
  Json entries = Json::array();
  for (const auto& e : I.entries)
    entries.push(Json::object({{"id", e.id}, {"sector", sector_word(e.sector)}, {"scope", static_cast<double>(e.scope)}, {"subsector", e.subsector}, {"gas", e.gas},
                               {"tonnes", e.key == ie::Key::estimated ? Json(ie::tonnes(I, e)) : Json()}, {"key", ie::code(e.key)},
                               {"basic", ie::in_level(e, ie::Level::basic)}, {"basic_plus", ie::in_level(e, ie::Level::basic_plus)},
                               {"territorial", ie::in_level(e, ie::Level::territorial)}, {"biogenic", e.biogenic}}));
  return Json::object({{"potentials", Json::object({{"name", I.potentials.name}, {"source", I.potentials.source}})},
                       {"basic", T.basic}, {"basic_plus", T.basic_plus}, {"territorial", T.territorial}, {"biogenic", T.biogenic},
                       {"u_basic", T.u_basic}, {"u_basic_plus", T.u_basic_plus}, {"u_territorial", T.u_territorial},
                       {"basic_complete", T.basic_complete}, {"basic_plus_complete", T.basic_plus_complete},
                       {"gaps_basic", strings(T.gaps_basic)}, {"gaps_basic_plus", strings(T.gaps_basic_plus)},
                       {"per_capita", maybe(T.per_capita)}, {"per_gdp", maybe(T.per_gdp)}, {"cells", cells}, {"gases", gases}, {"key_categories", keys}, {"entries", entries}});
}

inline Json curve_op(const Json& q) {
  const auto I = inventory_of(q["inventory"]);
  const auto ms = measures_of(q["measures"]);
  std::vector<std::string> order;
  for (const auto& o : items(q["order"])) order.push_back(o.text());
  const auto C = ie::curve(I, ms, level_of(q["level"].text("basic+")), order);
  Json rows = Json::array();
  for (const auto& a : C.measures)
    rows.push(Json::object({{"id", a.id}, {"name", a.name}, {"position", static_cast<double>(a.position)}, {"alone", a.alone}, {"in_order", a.in_order}, {"shapley", a.shapley},
                            {"cost", a.cost}, {"cost_per_tonne_alone", maybe(a.cost_per_tonne_alone)}, {"cost_per_tonne_in_order", maybe(a.cost_per_tonne_in_order)},
                            {"cost_per_tonne_shapley", maybe(a.cost_per_tonne_shapley)}}));
  return Json::object({{"baseline", C.baseline}, {"joint", C.joint}, {"sum_alone", C.sum_alone}, {"overlap", C.overlap}, {"measures", rows}});
}

inline Json pathway_op(const Json& q) {
  const auto I = inventory_of(q["inventory"]);
  const auto ms = measures_of(q["measures"]);
  std::map<ie::Sector, double> growth;
  if (q["growth"].is_object())
    for (const auto& [s, g] : q["growth"].as_object()) growth[sector_of(s)] = g.as_number();
  ie::Target t;
  const Json& tj = q["target"];
  t.year = static_cast<int>(num(tj["year"], 0)), t.reduction = num(tj["reduction"]), t.budget = num(tj["budget"]);
  const auto P = ie::pathway(I, ms, growth, static_cast<int>(num(q["last"], I.year + 10)), t, level_of(q["level"].text("basic+")));
  Json years = Json::array();
  for (const auto& y : P.years)
    years.push(Json::object({{"year", static_cast<double>(y.year)}, {"baseline", y.baseline}, {"with_measures", y.with_measures}, {"cost", y.cost}}));
  return Json::object({{"years", years}, {"cumulative_baseline", P.cumulative_baseline}, {"cumulative_with", P.cumulative_with}, {"target_level", maybe(P.target_level)},
                       {"gap", maybe(P.gap)}, {"meets_target", P.meets_target}, {"within_budget", P.within_budget}});
}

inline bool handles(const std::string& op) { return op.rfind("emissions.", 0) == 0; }

inline Json answer(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "emissions.inventory") return inventory_op(q);
  if (op == "emissions.curve") return curve_op(q);
  if (op == "emissions.pathway") return pathway_op(q);
  throw std::invalid_argument("emissions: no operation \"" + op + "\" (emissions.inventory, emissions.curve, emissions.pathway)");
}

}  // namespace science::emissions
