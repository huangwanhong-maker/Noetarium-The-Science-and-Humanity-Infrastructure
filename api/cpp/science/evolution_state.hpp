// The state of heuresis's evolution as JSON, and back, for any genotype (ADR 0127): what a search keeps between requests, so that
// a work may run it a few generations at a time and resume it exactly. The genotype is written and read by functions the caller
// gives. (The needs search's composition, science/needs.hpp, keeps its own copy of the same; it may move here.)

#pragma once

#include "json.hpp"

#include <heuresis/evolution/evolution.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace science::evolution_state {

namespace hev = heuresis::evolution;
using heuresis::search::Ticket;

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }

inline const Json::Array& items(const Json& j) {
  static const Json::Array none;
  return j.is_array() ? j.as_array() : none;
}
inline std::size_t count_of(const Json& j, const char* key) { return j[key].is_number() ? static_cast<std::size_t>(j[key].as_number()) : 0; }
inline std::vector<double> numbers(const Json& j) {
  std::vector<double> v;
  for (const auto& x : items(j)) v.push_back(x.is_number() ? x.as_number() : std::nan(""));
  return v;
}
template <class T>
inline std::vector<T> whole(const Json& j) {
  std::vector<T> v;
  for (const auto& x : items(j)) v.push_back(static_cast<T>(x.as_number()));
  return v;
}

inline Json evaluation_json(const heuresis::search::Evaluation& e) {
  Json j = Json::object({{"objectives", e.objectives}, {"constraints", e.constraints}, {"descriptors", e.descriptors}, {"cost", e.cost}});
  if (e.status == heuresis::search::Status::failed) j.set("failed", true), j.set("reason", e.reason);
  return j;
}
inline heuresis::search::Evaluation evaluation_of(const Json& j) {
  heuresis::search::Evaluation e;
  e.objectives = numbers(j["objectives"]), e.constraints = numbers(j["constraints"]), e.descriptors = numbers(j["descriptors"]);
  e.cost = j["cost"].is_number() ? j["cost"].as_number() : 1;
  if (j["failed"].is_bool() && j["failed"].as_bool()) e.status = heuresis::search::Status::failed, e.reason = j["reason"].text();
  return e;
}

inline Json statistics_json(const hev::Statistics& s) {
  Json p = Json::object(), q = Json::object();
  for (const auto& [k, v] : s.proposals) p.set(k, v);
  for (const auto& [k, v] : s.proposals_surviving) q.set(k, v);
  Json b = Json::array(), m = Json::array();
  for (double x : s.best) b.push(num(x));
  for (double x : s.median) m.push(num(x));
  return Json::object({{"generation", s.generation}, {"evaluations", s.evaluations}, {"best", b}, {"median", m}, {"diversity", num(s.diversity)},
                       {"births", std::vector<double>(s.births.begin(), s.births.end())}, {"survivals", std::vector<double>(s.survivals.begin(), s.survivals.end())},
                       {"probability", s.probability}, {"proposals", p}, {"proposals_surviving", q}, {"archive", s.archive}, {"admitted", s.admitted},
                       {"retired", s.retired}, {"failures", s.failures}, {"repeats", s.repeats}, {"reused", s.reused}, {"improved", s.improved}, {"restarted", s.restarted}});
}
inline hev::Statistics statistics_of(const Json& j) {
  hev::Statistics s;
  s.generation = count_of(j, "generation"), s.evaluations = count_of(j, "evaluations");
  s.best = numbers(j["best"]), s.median = numbers(j["median"]);
  s.diversity = j["diversity"].is_number() ? j["diversity"].as_number() : std::nan("");
  s.births = whole<std::size_t>(j["births"]), s.survivals = whole<std::size_t>(j["survivals"]), s.probability = numbers(j["probability"]);
  if (j["proposals"].is_object())
    for (const auto& [k, v] : j["proposals"].as_object()) s.proposals[k] = static_cast<std::size_t>(v.as_number());
  if (j["proposals_surviving"].is_object())
    for (const auto& [k, v] : j["proposals_surviving"].as_object()) s.proposals_surviving[k] = static_cast<std::size_t>(v.as_number());
  s.archive = count_of(j, "archive"), s.admitted = count_of(j, "admitted"), s.retired = count_of(j, "retired");
  s.failures = count_of(j, "failures"), s.repeats = count_of(j, "repeats"), s.reused = count_of(j, "reused");
  s.improved = j["improved"].is_bool() && j["improved"].as_bool(), s.restarted = j["restarted"].is_bool() && j["restarted"].as_bool();
  return s;
}

template <class G, class Enc>
inline Json member_json(const hev::Member<G>& m, Enc enc) {
  return Json::object({{"ticket", static_cast<double>(m.ticket)}, {"genotype", enc(m.genotype)}, {"evaluation", evaluation_json(m.evaluation)},
                       {"score", m.score}, {"operation", m.operation}, {"parents", std::vector<double>(m.parents.begin(), m.parents.end())},
                       {"born", m.born}, {"age", m.age}, {"layer", m.layer}, {"last_useful", m.last_useful}, {"novelty", num(m.novelty)}, {"restarted", m.restarted}});
}
template <class G, class Dec>
inline hev::Member<G> member_of(const Json& j, Dec dec) {
  hev::Member<G> m;
  m.ticket = static_cast<Ticket>(j["ticket"].as_number());
  m.genotype = dec(j["genotype"]);
  m.evaluation = evaluation_of(j["evaluation"]);
  m.score = numbers(j["score"]);
  m.operation = j["operation"].text();
  for (const auto& p : items(j["parents"])) m.parents.push_back(static_cast<Ticket>(p.as_number()));
  m.born = count_of(j, "born"), m.age = count_of(j, "age"), m.layer = count_of(j, "layer"), m.last_useful = count_of(j, "last_useful");
  m.novelty = j["novelty"].is_number() ? j["novelty"].as_number() : 0, m.restarted = j["restarted"].is_bool() && j["restarted"].as_bool();
  return m;
}

template <class G, class Enc>
inline Json state_json(const hev::State<G>& e, Enc enc) {
  Json pop = Json::array(), arc = Json::array(), ret = Json::array(), cache = Json::array(), stats = Json::array(), lin = Json::array();
  for (const auto& m : e.population) pop.push(member_json(m, enc));
  for (const auto& k : e.archive) arc.push(Json::object({{"member", member_json(k.member, enc)}, {"admitted", k.admitted}, {"note", k.note}}));
  for (const auto& r : e.retired) ret.push(Json::object({{"member", member_json(r.member, enc)}, {"admitted", r.admitted}, {"retired", r.retired}, {"note", r.note}, {"reason", r.reason}}));
  for (const auto& [k, ev] : e.cache) cache.push(Json::array({k, evaluation_json(ev)}));
  for (const auto& st : e.statistics) stats.push(statistics_json(st));
  for (const auto& l : e.lineage)
    lin.push(Json::array({static_cast<double>(l.ticket), l.generation, l.operation, std::vector<double>(l.parents.begin(), l.parents.end()), l.objectives,
                          (l.failed ? 1 : 0) + (l.survived ? 2 : 0) + (l.admitted ? 4 : 0)}));
  return Json::object({{"generation", e.generation}, {"evaluations", e.evaluations}, {"serial", e.serial}, {"stall", e.stall}, {"population", pop}, {"archive", arc},
                       {"retired", ret}, {"quality", e.quality}, {"probability", e.probability}, {"successes", e.successes}, {"failures", e.failures},
                       {"count", std::vector<double>(e.count.begin(), e.count.end())}, {"total", e.total}, {"cache", cache}, {"statistics", stats},
                       {"lineage", lin}, {"best", e.best ? member_json(*e.best, enc) : Json()}, {"stopped", e.stopped}, {"reseed", e.reseed}});
}

template <class G, class Dec>
inline hev::State<G> state_of(const Json& e, Dec dec) {
  hev::State<G> ev;
  ev.generation = count_of(e, "generation"), ev.evaluations = count_of(e, "evaluations"), ev.serial = count_of(e, "serial"), ev.stall = count_of(e, "stall");
  for (const auto& m : items(e["population"])) ev.population.push_back(member_of<G>(m, dec));
  for (const auto& k : items(e["archive"])) ev.archive.push_back({member_of<G>(k["member"], dec), count_of(k, "admitted"), k["note"].text()});
  for (const auto& r : items(e["retired"])) ev.retired.push_back({member_of<G>(r["member"], dec), count_of(r, "admitted"), count_of(r, "retired"), r["note"].text(), r["reason"].text()});
  ev.quality = numbers(e["quality"]), ev.probability = numbers(e["probability"]), ev.successes = numbers(e["successes"]), ev.failures = numbers(e["failures"]);
  ev.count = whole<std::size_t>(e["count"]), ev.total = count_of(e, "total");
  for (const auto& c : items(e["cache"])) ev.cache.emplace_back(c[0].as_string(), evaluation_of(c[1]));
  for (const auto& st : items(e["statistics"])) ev.statistics.push_back(statistics_of(st));
  for (const auto& l : items(e["lineage"])) {
    hev::Lineage x;
    x.ticket = static_cast<Ticket>(l[0].as_number()), x.generation = static_cast<std::size_t>(l[1].as_number()), x.operation = l[2].as_string();
    for (const auto& p : items(l[3])) x.parents.push_back(static_cast<Ticket>(p.as_number()));
    x.objectives = numbers(l[4]);
    const int f = static_cast<int>(l[5].as_number());
    x.failed = f & 1, x.survived = f & 2, x.admitted = f & 4;
    ev.lineage.push_back(std::move(x));
  }
  if (e["best"].is_object()) ev.best = member_of<G>(e["best"], dec);
  ev.stopped = e["stopped"].is_bool() && e["stopped"].as_bool(), ev.reseed = e["reseed"].is_bool() && e["reseed"].as_bool();
  return ev;
}

}  // namespace science::evolution_state
