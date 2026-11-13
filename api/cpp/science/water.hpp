// Water networks, for any work's engine (ADR 0107): one composition of the civil-engineering domain's `water`, answering requests in
// JSON. A network is given in the request as the text of an EPANET input file; nothing here names a city or a utility.
//
//   water_solve        {"inp": text, "time"?: s, "pressure_driven"?: false, "minimum_pressure"?: m, "required_pressure"?: m,
//                       "closed"?: [link ids]}
//                        the heads, pressures, delivered demands and flows at an instant, the age of the water, Todini's resilience
//                        index at the required pressure, and the junctions isolated
//   water_simulate     {"inp": text, "pressure_driven"?, ...}   the period the file declares, at its hydraulic step: each node's pressure
//                        and each tank's level by hour, the demand delivered and asked
//   water_criticality  {"inp": text, "links"?: [ids] (default: every pipe), "required_pressure"?: m, "minimum_pressure"?: m}
//                        each link closed in turn under pressure-driven demand: the share delivered and the junctions left below
//
// Units: SI (m, m³/s, s), whatever the file's units; pressures in m of water (× 9.80665 for kPa).

#pragma once

#include <civil_engineering/water/water.hpp>

#include "json.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::water {

namespace cw = ::civil_engineering::water;

inline Json nums(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(std::isfinite(x) ? Json(x) : Json());
  return a;
}
inline Json words(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(Json(x));
  return a;
}
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }

inline cw::Network network_of(const Json& r) {
  if (!r["inp"].is_string()) throw std::invalid_argument("water: the request holds an EPANET input file's text as \"inp\"");
  std::istringstream in(r["inp"].as_string());
  return cw::read_inp(in);
}
inline cw::Options options_of(const Json& r) {
  cw::Options o;
  o.time = number_or(r, "time", 0);
  o.pressure_driven = r.has("pressure_driven") && r["pressure_driven"].as_bool();
  o.minimum_pressure = number_or(r, "minimum_pressure", 0);
  o.required_pressure = number_or(r, "required_pressure", 20);
  for (std::size_t i = 0; i < r["closed"].size(); i++) o.open[r["closed"][i].text()] = false;
  return o;
}

inline Json snapshot_json(const cw::Network& net, const cw::Snapshot& s, double required) {
  std::vector<std::string> kinds;
  for (std::size_t v = 0; v < s.nodes.size(); v++) kinds.push_back(v < net.junctions.size() ? "junction" : v < net.junctions.size() + net.reservoirs.size() ? "reservoir" : "tank");
  std::vector<std::string> link_kinds;
  for (std::size_t e = 0; e < s.links.size(); e++) link_kinds.push_back(e < net.pipes.size() ? "pipe" : "pump");
  std::vector<double> open;
  for (bool b : s.open) open.push_back(b ? 1 : 0);
  const auto res = cw::resilience(net, s, required);
  return Json::object({{"nodes", words(s.nodes)}, {"kind", words(kinds)}, {"elevation", nums(s.elevation)}, {"head", nums(s.head)}, {"pressure", nums(s.pressure)},
                       {"demand", nums(s.demand)}, {"required", nums(s.required)}, {"links", words(s.links)}, {"link_kind", words(link_kinds)},
                       {"flow", nums(s.flow)}, {"velocity", nums(s.velocity)}, {"headloss", nums(s.headloss)}, {"open", nums(open)},
                       {"isolated", words(s.isolated)}, {"converged", Json(s.converged)}, {"residual", Json(s.residual)}, {"balance", Json(s.balance)},
                       {"resilience", std::isfinite(res.index) ? Json(res.index) : Json()}, {"deficient", Json(static_cast<double>(res.deficient))}});
}

inline Json op_solve(const Json& r) {
  const auto net = network_of(r);
  const auto o = options_of(r);
  const auto s = cw::solve(net, o);
  Json out = snapshot_json(net, s, o.required_pressure);
  out.set("age", nums(cw::age(net, s)));
  out.set("title", Json(net.title));
  return out;
}

inline Json op_simulate(const Json& r) {
  const auto net = network_of(r);
  const auto o = options_of(r);
  const auto series = cw::simulate(net, o);
  Json times = nums(series.times), pressure = Json::array(), delivered = Json::array(), asked = Json::array(), levels = Json::object();
  for (const auto& s : series.snapshots) {
    pressure.push(nums(s.pressure));
    double d = 0, q = 0;
    for (std::size_t i = 0; i < net.junctions.size(); i++) d += s.demand[i], q += s.required[i];
    delivered.push(Json(d)), asked.push(Json(q));
  }
  for (const auto& [id, lv] : series.tank_level) levels.set(id, nums(lv));
  std::vector<std::string> kinds;
  for (std::size_t v = 0; v < series.snapshots.front().nodes.size(); v++)
    kinds.push_back(v < net.junctions.size() ? "junction" : v < net.junctions.size() + net.reservoirs.size() ? "reservoir" : "tank");
  return Json::object({{"times", times}, {"nodes", words(series.snapshots.front().nodes)}, {"kind", words(kinds)}, {"pressure", pressure}, {"delivered", delivered},
                       {"asked", asked}, {"tank_level", levels}, {"step", Json(net.hydraulic_step)}});
}

inline Json op_criticality(const Json& r) {
  const auto net = network_of(r);
  auto o = options_of(r);
  std::vector<std::string> links;
  for (std::size_t i = 0; i < r["links"].size(); i++) links.push_back(r["links"][i].text());
  if (links.empty())
    for (const auto& p : net.pipes) links.push_back(p.id);
  const auto crit = cw::criticality(net, o, links);
  std::vector<std::string> ids;
  std::vector<double> delivered, below;
  for (const auto& c : crit) ids.push_back(c.link), delivered.push_back(c.delivered), below.push_back(static_cast<double>(c.below));
  return Json::object({{"links", words(ids)}, {"delivered", nums(delivered)}, {"below", nums(below)}, {"required_pressure", Json(o.required_pressure)}});
}

inline bool handles(const std::string& op) { return op.rfind("water_", 0) == 0; }

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "water_solve") return op_solve(r);
  if (op == "water_simulate") return op_simulate(r);
  if (op == "water_criticality") return op_criticality(r);
  throw std::invalid_argument("water: no operation " + op + " (water_solve, water_simulate, water_criticality)");
}

}  // namespace science::water
