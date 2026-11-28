// Measures of power and value readings of a community's record (Phase 9 of the design *Communities That Agents Of Any Provider Find,
// Join, Work In And Leave*, Sections 4.5 and 4.6): the communities domain's trajectories, which rest on nomos and tekmerion, answered by
// an engine of their own (api/cpp/engines/community-readings/main.cpp) so that the engine of sessions stays small (Section 9.7). A
// session's record, seals and behaviour journal are what the API's Python sends here.
//
//   communities.power   {constitution, parties, acts, mechanism: {name, parameters}}   a measure of power (Section 4.6), a declared
//                                                                 mechanism applied to the record from its founding, with the
//                                                                 mechanism's digest and the head it read
//   communities.reading {constitution, parties, acts, mechanism, journal?, seals?}    a value reading (Definition 33) through the join of
//                                                                 the chronicle and the behaviour journal, acts decided under a reading
//                                                                 left out (H7); with seals, whether the record replays as they say
#pragma once

#include <communities/trajectories/trajectories.hpp>

#include <science/communities.hpp>

namespace science::communities::readings {

namespace tr = ::communities::trajectories;

inline tr::Mechanism mechanism_of(const Json& j) {
  tr::Mechanism m;
  m.name = j.is_string() ? j.text() : j["name"].text();
  if (j["parameters"].is_object())
    for (const auto& [k, v] : j["parameters"].as_object()) m.parameters[k] = scalar(v);
  if (m.name.empty()) throw std::invalid_argument("communities: a mechanism is named");
  return m;
}
inline Json mechanism_json(const tr::Mechanism& m) {
  Json p = Json::object();
  for (const auto& [k, v] : m.parameters) p.set(k, v);
  return Json::object({{"name", m.name}, {"parameters", p}});
}

inline Json power_op(const Json& q) {
  const auto m = mechanism_of(q["mechanism"]);
  tr::Record rec;
  rec.parties = parties_of(q["parties"]), rec.founding = constitution_of(q["constitution"]);
  for (const auto& a : list(q["acts"])) rec.acts.push_back(act_of(a));
  const auto r = tr::measure(m, rec);
  Json counts = Json::object(), shares = Json::object(), banzhaf = Json::object(), shapley = Json::object();
  for (const auto& [k, n] : r.counts) counts.set(k, static_cast<double>(n));
  for (const auto& [k, x] : r.shares) shares.set(k, x.str());
  for (const auto& [k, x] : r.banzhaf) banzhaf.set(k, x);
  for (const auto& [k, x] : r.shapley_shubik) shapley.set(k, x);
  return Json::object({{"mechanism", mechanism_json(r.mechanism)}, {"mechanism digest", r.mechanism_digest}, {"head", r.head}, {"size", static_cast<double>(r.size)},
                       {"counts", counts}, {"shares", shares}, {"herfindahl", r.herfindahl}, {"banzhaf", banzhaf}, {"shapley_shubik", shapley},
                       {"notes", strings(r.notes)}, {"refused", strings(r.refused)}});
}

inline Json reading_op(const Json& q) {
  const auto m = mechanism_of(q["mechanism"]);
  const ch::State s = folded(q);
  std::optional<ch::Conduct> c;
  if (q["journal"].is_array()) c.emplace(), c->journal = journal_of(q["journal"]);
  const auto r = tr::read_values(m, s, c ? &*c : nullptr);
  Json result = Json::object(), excluded = Json::array();
  for (const auto& [k, v] : r.result) result.set(k, v);
  for (auto p : r.excluded) excluded.push(static_cast<double>(p));
  Json out = Json::object({{"mechanism", mechanism_json(r.mechanism)}, {"mechanism digest", r.mechanism_digest},
                           {"chronicle", Json::object({{"community", r.community}, {"size", static_cast<double>(r.size)}, {"root", r.root}})},
                           {"journal", Json::object({{"size", static_cast<double>(r.journal_size)}, {"seal", r.journal_seal}})}, {"join", r.join},
                           {"excluded", excluded}, {"episodes left out", static_cast<double>(r.episodes_left_out)}, {"result", result}, {"grade", r.grade},
                           {"digest", r.digest}, {"refused", strings(r.refused)}});
  replays_into(out, q["seals"], s);
  return out;
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (op == "communities.power") return power_op(q);
  if (op == "communities.reading") return reading_op(q);
  throw std::invalid_argument("communities: the engine of readings answers communities.power and communities.reading, not \"" + op + "\"");
}

}  // namespace science::communities::readings
