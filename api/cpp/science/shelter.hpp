// Households in slums, informal settlements and inadequate housing, as one composition any work may call (ADR 0155): urban science's
// `shelter` on a household survey read under declared readings of SDG 11.1.1's open terms. Nothing in it names a city.
//
//   shelter.assess {households: [{weight, stratum, cluster, group?, persons, rooms?, water?, sanitation?, shared_toilet?, walls?, roof?,
//                    floor?, hazardous_site?, documented?, fears_eviction?, housing_cost?, income?}],
//                   readings: [{name, water_improved: [code], sanitation_improved: [code], shared_deprives?, most_per_room?, durable: [code],
//                   parts?: ["walls", "roof", "floor"], hazard_deprives?, tenure?: "left out" | "documented" | "documented and unthreatened" |
//                   "documented or unthreatened", afford?, unknowns?: "left out" | "deprived" | "not deprived", source?}], confidence?}
//                  each reading's shares of people in slum or informal households, in inadequate housing and in either, with intervals and
//                  bounds over missing answers; each component's share; people deprived of one alone; people by number of deprivations;
//                  the slum share by group
//
// A missing answer is an absent key, null, an empty code or a negative flag. Shares are of people, weighted by the survey's weights.

#pragma once

#include <urban/shelter/shelter.hpp>

#include <science/json.hpp>

#include <cmath>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::shelter {

namespace us = urban::shelter;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }
inline int flag(const Json& j) {
  if (j.is_bool()) return j.as_bool() ? 1 : 0;
  if (j.is_number()) return j.as_number() < 0 ? -1 : j.as_number() > 0 ? 1 : 0;
  return -1;
}
inline std::set<std::string> codes(const Json& a) {
  std::set<std::string> s;
  if (a.is_array())
    for (const auto& x : a.as_array()) s.insert(x.text());
  return s;
}

inline us::Household household_of(const Json& h) {
  us::Household x;
  x.weight = num(h["weight"], 1), x.stratum = static_cast<int>(num(h["stratum"], 0)), x.cluster = static_cast<int>(num(h["cluster"], 0));
  x.group = h["group"].text(""), x.persons = num(h["persons"], 0), x.rooms = num(h["rooms"], us::unknown);
  x.water = h["water"].text(""), x.sanitation = h["sanitation"].text(""), x.shared_toilet = flag(h["shared_toilet"]);
  x.walls = h["walls"].text(""), x.roof = h["roof"].text(""), x.floor = h["floor"].text("");
  x.hazardous_site = flag(h["hazardous_site"]), x.documented = flag(h["documented"]), x.fears_eviction = flag(h["fears_eviction"]);
  x.housing_cost = num(h["housing_cost"], us::unknown), x.income = num(h["income"], us::unknown);
  return x;
}

inline us::Reading reading_of(const Json& r) {
  us::Reading R;
  R.name = r["name"].text("reading");
  R.water_improved = codes(r["water_improved"]), R.sanitation_improved = codes(r["sanitation_improved"]), R.durable = codes(r["durable"]);
  if (r["parts"].is_array()) R.parts = codes(r["parts"]);
  R.shared_deprives = r["shared_deprives"].is_bool() && r["shared_deprives"].as_bool();
  R.hazard_deprives = r["hazard_deprives"].is_bool() && r["hazard_deprives"].as_bool();
  R.most_per_room = num(r["most_per_room"], 3), R.afford = num(r["afford"], 0.30);
  const std::string t = r["tenure"].text("left out");
  if (t == "left out") R.tenure = us::Tenure::left_out;
  else if (t == "documented") R.tenure = us::Tenure::documented;
  else if (t == "documented and unthreatened") R.tenure = us::Tenure::documented_and_unthreatened;
  else if (t == "documented or unthreatened") R.tenure = us::Tenure::documented_or_unthreatened;
  else throw std::invalid_argument("shelter: tenure is left out, documented, documented and unthreatened, or documented or unthreatened, not " + t);
  const std::string u = r["unknowns"].text("left out");
  if (u == "left out") R.unknowns = us::Unknown::left_out;
  else if (u == "deprived") R.unknowns = us::Unknown::deprived;
  else if (u == "not deprived") R.unknowns = us::Unknown::not_deprived;
  else throw std::invalid_argument("shelter: an unknown answer is left out, deprived or not deprived, not " + u);
  R.source = r["source"].text("the request");
  return R;
}

inline Json share_json(const us::Share& s) {
  return Json::object({{"value", finite(s.estimate.value)}, {"se", finite(s.estimate.se)}, {"ci_lower", finite(s.estimate.ci_lower)},
                       {"ci_upper", finite(s.estimate.ci_upper)}, {"lower", s.lower}, {"upper", s.upper}, {"unknown", s.unknown_people}});
}

inline Json assess(const Json& r) {
  std::vector<us::Household> hs;
  for (const auto& h : r["households"].as_array()) hs.push_back(household_of(h));
  const double confidence = num(r["confidence"], 0.95);
  Json out = Json::array();
  for (const auto& x : r["readings"].as_array()) {
    const auto R = reading_of(x);
    const auto a = us::assess(hs, R, confidence);
    Json comps = Json::array(), only = Json::array(), deps = Json::array(), groups = Json::object();
    for (std::size_t c = 0; c < 5; c++) {
      Json j = share_json(a.component[c]);
      j.set("component", Json(std::string(us::components[c])));
      comps.push(j);
      only.push(Json(a.only[c]));
    }
    for (const double v : a.deprivations) deps.push(Json(v));
    for (const auto& [g, s] : a.slum_by_group) groups.set(g, share_json(s));
    out.push(Json::object({{"reading", a.reading}, {"source", R.source}, {"people", a.people}, {"households", a.households}, {"slum", share_json(a.slum)},
                           {"inadequate", share_json(a.inadequate)}, {"either", share_json(a.either)}, {"components", comps}, {"only", only},
                           {"deprivations", deps}, {"slum_by_group", groups}, {"tenure_counted", R.tenure != us::Tenure::left_out}}));
  }
  return Json::object({{"readings", out}, {"households", static_cast<double>(hs.size())}});
}

inline bool handles(const std::string& op) { return op.rfind("shelter.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "shelter.assess") return assess(r);
  throw std::invalid_argument("shelter: no operation " + op);
}

}  // namespace science::shelter
