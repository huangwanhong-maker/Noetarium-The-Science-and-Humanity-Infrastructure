// A city's municipal solid waste and SDG 11.6.1 as one composition any work may call (ADR 0148): the industrial-ecology domain's
// `waste`, read and answered as JSON. Every ladder, amount and design is the requester's declaration.
//
//   waste.account  {survey: {households: {stratum: H}, sample: [{stratum, area, persons, kg, days}], confidence}, other: {source: t a day},
//                   facilities: [{id, name, kind: recovery | disposal, received, recovered, residue_to, met: [criterion]}],
//                   ladder: {levels: [name], criteria: [[criterion]], controlled_from}, not_delivered, sorted: [{stratum, area, kg: {fraction: kg}}]}
//                  the generation with its interval, per person by stratum, each facility's level of control and what it manages, the
//                  account from generated to managed in controlled facilities, SDG 11.6.1 with its interval, coverage, recovery, the
//                  imbalance and the remarks, and the composition when samples are sorted
#pragma once

#include <industrial_ecology/waste/waste.hpp>

#include <science/json.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::waste {

namespace iw = industrial_ecology::waste;

inline const Json::Array& items(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}
inline double num(const Json& j, double fallback = iw::none) { return j.is_number() ? j.as_number() : fallback; }
inline Json maybe(double x) { return std::isfinite(x) ? Json(x) : Json(); }
inline Json estimate(const peira::surveys::Estimate& e) {
  return Json::object({{"value", e.value}, {"se", e.se}, {"df", e.df}, {"lower", e.ci_lower}, {"upper", e.ci_upper}});
}

inline std::map<int, double> strata_of(const Json& j) {
  std::map<int, double> out;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object()) out[std::stoi(k)] = v.as_number();
  return out;
}

inline Json account_op(const Json& q) {
  const Json& sj = q["survey"];
  iw::Survey S;
  S.households = strata_of(sj["households"]);
  S.confidence = num(sj["confidence"], 0.95);
  for (const auto& h : items(sj["sample"]))
    S.sample.push_back({static_cast<int>(num(h["stratum"], 0)), static_cast<int>(num(h["area"], 0)), num(h["persons"], 0), num(h["kg"], 0), num(h["days"], 0)});
  std::map<std::string, double> other;
  if (q["other"].is_object())
    for (const auto& [k, v] : q["other"].as_object()) other[k] = v.as_number();
  const auto G = iw::generation(S, other);
  iw::Ladder L;
  for (const auto& l : items(q["ladder"]["levels"])) L.levels.push_back(l.text());
  for (const auto& c : items(q["ladder"]["criteria"])) {
    std::vector<std::string> cs;
    for (const auto& x : items(c)) cs.push_back(x.text());
    L.criteria.push_back(cs);
  }
  L.controlled_from = static_cast<std::size_t>(num(q["ladder"]["controlled_from"], 0));
  std::vector<iw::Facility> fs;
  for (const auto& f : items(q["facilities"])) {
    iw::Facility x;
    x.id = f["id"].text(), x.name = f["name"].text(x.id);
    const std::string kind = f["kind"].text("disposal");
    if (kind != "recovery" && kind != "disposal") throw std::invalid_argument("waste: facility " + x.id + " is a recovery or a disposal facility");
    x.kind = kind == "recovery" ? iw::Kind::recovery : iw::Kind::disposal;
    x.received = num(f["received"], 0), x.recovered = num(f["recovered"], 0), x.residue_to = f["residue_to"].text();
    for (const auto& m : items(f["met"])) x.met.insert(m.text());
    fs.push_back(x);
  }
  const auto A = iw::account(G, fs, L, num(q["not_delivered"], 0));
  Json facilities = Json::array();
  for (const auto& r : A.facilities)
    facilities.push(Json::object({{"id", r.id}, {"level", static_cast<double>(r.level)}, {"level_name", r.level_name}, {"controlled", r.controlled}, {"received", r.received},
                                  {"residues_in", r.residues_in}, {"from_collection", r.from_collection}, {"recovered", r.recovered}, {"residue_out", r.residue_out},
                                  {"managed_controlled", r.managed_controlled}}));
  Json by = Json::object();
  for (const auto& [s, v] : G.per_person_by_stratum) by.set(std::to_string(s), v);
  Json remarks = Json::array();
  for (const auto& r : A.remarks) remarks.push(r);
  Json out = Json::object({{"households", estimate(G.households)}, {"per_person", estimate(G.per_person)}, {"per_person_by_stratum", by}, {"other", G.other},
                           {"generated", G.total}, {"generated_lower", G.lower}, {"generated_upper", G.upper}, {"collected", A.collected}, {"uncollected", A.uncollected},
                           {"delivered", A.delivered}, {"not_delivered", A.not_delivered}, {"recovered", A.recovered}, {"recovered_controlled", A.recovered_controlled},
                           {"disposed", A.disposed}, {"disposed_controlled", A.disposed_controlled}, {"managed_controlled", A.managed_controlled},
                           {"indicator", maybe(A.indicator)}, {"indicator_lower", maybe(A.indicator_lower)}, {"indicator_upper", maybe(A.indicator_upper)},
                           {"collection_coverage", maybe(A.collection_coverage)}, {"recovery_rate", maybe(A.recovery_rate)}, {"imbalance", A.imbalance},
                           {"facilities", facilities}, {"remarks", remarks}});
  if (!items(q["sorted"]).empty()) {
    std::vector<iw::Sorted> sorted;
    for (const auto& x : items(q["sorted"])) {
      iw::Sorted s{static_cast<int>(num(x["stratum"], 0)), static_cast<int>(num(x["area"], 0)), {}};
      if (x["kg"].is_object())
        for (const auto& [f, k] : x["kg"].as_object()) s.kg[f] = k.as_number();
      sorted.push_back(s);
    }
    Json shares = Json::array();
    for (const auto& c : iw::composition(sorted, S.households, S.confidence)) {
      Json e = estimate(c.share);
      e.set("fraction", c.fraction);
      shares.push(e);
    }
    out.set("composition", shares);
  }
  return out;
}

inline bool handles(const std::string& op) { return op.rfind("waste.", 0) == 0; }

inline Json answer(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "waste.account") return account_op(q);
  throw std::invalid_argument("waste: no operation \"" + op + "\" (waste.account)");
}

}  // namespace science::waste
