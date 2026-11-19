// Multilateral practice for any work's engine (ADR 0132): one composition of the diplomacy domain with nomos (voting power,
// bargaining, the core and the nucleolus) and tyche (ideal points), answering requests in JSON. A request holds everything it needs;
// nothing is read from a file or kept between requests.
//
//   undertaking   {undertaking, at, thresholds?}   an undertaking among many parties read on a date (diplomacy::relations):
//                 its faults, the pairs of each type of relation by status (present, absent, not observed), every participant's
//                 position, its diagnostics, and its timeline (every tie begun or ended, every decision, every design of observation)
//   partnership   {partnership, at, standard, thresholds?, scheme?, alternatives?, rule?}   a partnership's record read on a date
//                 (diplomacy::partnerships): its faults; each commitment screened (SMART unless a scheme names fewer criteria) and its
//                 state; contributions by party, kind and unit; each party's values and benefit as a dual verdict, and mutual benefit by
//                 the declared standard; the signs of failure; and, when alternatives are given, each pair compared as scales permit,
//                 the undominated ones, and their sums by a declared rule where the rule's scales allow
//   rule          {members, rule, sizes?}   a decision rule's a priori power (nomos::games::power) and its properties; with each
//                 member's population, the indirect power of its citizens
//   ideal_points  {members, items, votes, anchor, bootstrap?, level?, seed?}   ideal points from votes (tyche::latent), with
//                 intervals by a parametric bootstrap when asked
//   bargaining    {outcomes, disagreement}   the Nash, Kalai–Smorodinsky and egalitarian solutions, the ideal point and the frontier
//   division      {issues, a, b}   Adjusted Winner between two parties who spread 100 points each
//   coalitional   {members, worths}   a joint gain divided: the Shapley value, the least core, whether the core is empty, the nucleolus
//   outcome, rule_game, negotiation, events, mandates, reviews, institutions   the affairs of an undertaking (ADR 0134), composed in
//                 science/affairs.hpp; mandates read against an undertaking when the request holds one
//
// The undertaking, noetarium.undertaking/1:
//   {name, types: [{name, meaning, authority, evidential, challenge}], participants: [{id, name, kind, constituted}],
//    ties: [{type, from, to, since, until, absent, source}], observations: [{type, design, since, until, covered}],
//    decisions: [{id, label, when, rule, closure, authorities, evidence, affected, venues, reason}], constraints: [{id, text, held_by}]}
//
// The partnership, noetarium.partnership/1:
//   {name, registered, members: [{party, role, since, until}], goals: [{id, text, set_by}],
//    commitments: [{id, by, text, goal, made, due, roles: {party: role}, indicator: {name, unit, baseline, target, higher_is_better},
//                   resources: [{party, kind, unit, amount, secured}], amendments: [{when, by, reason, target, due, transferred_to}],
//                   achievable: {by, reason, judged}}],
//    reports: [{commitment, when, by, source, value}], contributions: [{party, commitment, goal, kind, unit, when, source, amount}],
//    dimensions: [{name, kind, unit, scale, higher_is_better, levels}],
//    values: [{party, dimension, when, reading, source, value, lower, upper, error, grade}],
//    interests: [{party, dimension, reading, source, threshold}]}
//
// A rule: {"weights": [...], "quota": q} | {"at_least": k} (members counted) | {"all": [member, …]} (each a veto) |
// {"all_of": [rule, …]} | {"any_of": [rule, …]}. A weighted rule on more than 24 members is computed by generating functions; any
// other rule on at most 24 members by enumeration.
//
// Every number with no value (no observation, no estimate) is written null. The methods are the domain's and the foundations';
// the engine only reads, calls and writes.

#pragma once

#include "affairs.hpp"
#include "json.hpp"

#include <diplomacy/partnerships/partnerships.hpp>
#include <diplomacy/relations/relations.hpp>
#include <nomos/games/bargaining.hpp>
#include <nomos/games/claims.hpp>
#include <nomos/games/nucleolus.hpp>
#include <nomos/games/power.hpp>
#include <tyche/latent/ideal_points.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::multilateral {

namespace dr = ::diplomacy::relations;
namespace dp = ::diplomacy::partnerships;
namespace ng = ::nomos::games;
namespace kk = ::kybernesis::kernel;
using apodeixis::kernel::Verdict;

// ---- Small readers and writers -------------------------------------------------------------------------------------------------

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }
inline double number(const Json& j, double fallback = std::numeric_limits<double>::quiet_NaN()) { return j.is_number() ? j.as_number() : fallback; }
inline bool flag(const Json& j, bool fallback = false) { return j.is_bool() ? j.as_bool() : fallback; }

inline Json strings(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& s : v) a.push(s);
  return a;
}

inline std::vector<std::string> strings_of(const Json& j) {
  std::vector<std::string> out;
  for (std::size_t k = 0; k < j.size(); k++)
    if (j[k].is_string()) out.push_back(j[k].as_string());
  return out;
}

inline Json verdict_json(const Verdict& v) {
  Json evidence = Json::array();
  for (const auto& e : v.evidence) evidence.push(Json::object({{"role", e.role}, {"value", e.value}}));
  return Json::object({{"status", std::string(apodeixis::kernel::word(v.status))}, {"claim", v.claim}, {"reason", v.reason}, {"evidence", evidence}});
}

inline Json problems_json(const std::vector<dr::Problem>& ps) {
  Json a = Json::array();
  for (const auto& p : ps) a.push(Json::object({{"where", p.where}, {"what", p.what}}));
  return a;
}

inline Json problems_json(const std::vector<dp::Problem>& ps) {
  Json a = Json::array();
  for (const auto& p : ps) a.push(Json::object({{"where", p.where}, {"what", p.what}}));
  return a;
}

// ---- The undertaking -----------------------------------------------------------------------------------------------------------

inline dr::Undertaking undertaking_of(const Json& j) {
  dr::Undertaking u;
  u.name = j["name"].text();
  for (std::size_t k = 0; k < j["types"].size(); k++) {
    const Json& t = j["types"][k];
    u.types.push_back({t["name"].text(), t["meaning"].text(), flag(t["authority"]), flag(t["evidential"]), flag(t["challenge"])});
  }
  for (std::size_t k = 0; k < j["participants"].size(); k++) {
    const Json& p = j["participants"][k];
    u.participants.push_back({p["id"].text(), p["name"].text(), p["kind"].text(), flag(p["constituted"])});
  }
  for (std::size_t k = 0; k < j["ties"].size(); k++) {
    const Json& t = j["ties"][k];
    u.ties.push_back({t["type"].text(), t["from"].text(), t["to"].text(), t["since"].text(), t["until"].text(), flag(t["absent"]), t["source"].text()});
  }
  for (std::size_t k = 0; k < j["observations"].size(); k++) {
    const Json& o = j["observations"][k];
    u.observations.push_back({o["type"].text(), o["design"].text(), o["since"].text(), o["until"].text(), strings_of(o["covered"])});
  }
  for (std::size_t k = 0; k < j["decisions"].size(); k++) {
    const Json& d = j["decisions"][k];
    u.decisions.push_back({d["id"].text(), d["label"].text(), d["when"].text(), d["rule"].text(), d["closure"].text("provisional"), strings_of(d["authorities"]),
                           strings_of(d["evidence"]), strings_of(d["affected"]), strings_of(d["venues"]), d["reason"].text()});
  }
  for (std::size_t k = 0; k < j["constraints"].size(); k++) {
    const Json& c = j["constraints"][k];
    u.constraints.push_back({c["id"].text(), c["text"].text(), c["held_by"].text()});
  }
  return u;
}

inline Json map_json(const std::map<std::string, std::vector<std::string>>& m) {
  Json o = Json::object();
  for (const auto& [k, v] : m) o.set(k, strings(v));
  return o;
}

inline Json undertaking(const Json& r) {
  const dr::Undertaking u = undertaking_of(r["undertaking"]);
  const std::string at = r["at"].text();
  dr::Thresholds th;
  if (r["thresholds"].is_object()) {
    th.concentration = number(r["thresholds"]["concentration"], th.concentration);
    th.ai_kind = r["thresholds"]["ai_kind"].text(th.ai_kind);
  }
  Json out = Json::object();
  out.set("name", u.name);
  out.set("at", at);
  out.set("problems", problems_json(dr::problems(u)));
  Json layers = Json::array();
  for (const auto& t : u.types) {
    const auto l = dr::layer(u, t.name, at);
    layers.push(Json::object({{"type", t.name}, {"meaning", t.meaning}, {"authority", t.authority}, {"evidential", t.evidential}, {"challenge", t.challenge},
                              {"present", static_cast<double>(l.present)}, {"absent", static_cast<double>(l.absent)}, {"not_observed", static_cast<double>(l.not_observed)},
                              {"observed", num(l.observed())}}));
  }
  out.set("layers", layers);
  Json positions = Json::array();
  for (const auto& p : u.participants) {
    const auto q = dr::position(u, p.id, at);
    positions.push(Json::object({{"participant", p.id}, {"name", p.name}, {"kind", p.kind}, {"constituted", q.constituted}, {"formal_authority", q.formal_authority()},
                                 {"out", map_json(q.out)}, {"in", map_json(q.in)}, {"authorised_by", strings(q.authorised_by)}, {"relies_on", strings(q.relies_on)},
                                 {"relied_on_by", strings(q.relied_on_by)}, {"may_challenge", strings(q.may_challenge)}, {"decides", strings(q.decides)},
                                 {"affected_by", strings(q.affected_by)}, {"venue_of", strings(q.venue_of)}, {"constraints", strings(q.constraints_held)}}));
  }
  out.set("positions", positions);
  Json diagnoses = Json::array();
  for (const auto& d : dr::diagnose(u, at, th)) diagnoses.push(Json::object({{"kind", d.kind}, {"where", d.where}, {"what", d.what}}));
  out.set("diagnoses", diagnoses);
  // The timeline: what began, ended or was decided, in the order of dates.
  struct Event {
    std::string date, kind, what;
    std::vector<std::string> participants;
  };
  std::vector<Event> events;
  for (const auto& t : u.ties) {
    const std::string tie = t.from + " " + t.type + " " + t.to;
    if (!t.since.empty()) events.push_back({t.since, t.absent ? "recorded absent" : "tie begins", tie, {t.from, t.to}});
    if (!t.until.empty()) events.push_back({t.until, "tie ends", tie, {t.from, t.to}});
  }
  for (const auto& d : u.decisions) {
    std::vector<std::string> who = d.authorities;
    for (const auto& a : d.affected) who.push_back(a);
    if (!d.when.empty()) events.push_back({d.when, "decision", d.id + (d.label.empty() ? "" : ": " + d.label), who});
  }
  for (const auto& o : u.observations) {
    if (!o.since.empty()) events.push_back({o.since, "observed from", o.design + " (" + o.type + ")", o.covered});
    if (!o.until.empty()) events.push_back({o.until, "observed until", o.design + " (" + o.type + ")", o.covered});
  }
  std::stable_sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.date < b.date; });
  Json timeline = Json::array();
  for (const auto& e : events) timeline.push(Json::object({{"date", e.date}, {"kind", e.kind}, {"what", e.what}, {"participants", strings(e.participants)}}));
  out.set("timeline", timeline);
  return out;
}

// ---- The partnership -----------------------------------------------------------------------------------------------------------

inline kk::Scale scale_of(const std::string& s) {
  try {
    return kk::scale_of(s.empty() ? "ratio" : s);
  } catch (const std::exception&) {
    throw std::invalid_argument("science::multilateral: no scale named " + s + " (nominal, ordinal, interval, ratio, absolute)");
  }
}

inline dp::Value value_of(const Json& v) {
  dp::Value x;
  x.party = v["party"].text(), x.dimension = v["dimension"].text(), x.when = v["when"].text(), x.reading = v["reading"].text(), x.source = v["source"].text();
  x.value = number(v["value"]), x.lower = number(v["lower"]), x.upper = number(v["upper"]), x.error = number(v["error"]);
  const std::string g = v["grade"].text("A");
  x.grade = g.empty() ? 'A' : g[0];
  return x;
}

inline dp::Partnership partnership_of(const Json& j) {
  dp::Partnership p;
  p.name = j["name"].text(), p.registered = j["registered"].text();
  for (std::size_t k = 0; k < j["members"].size(); k++) {
    const Json& m = j["members"][k];
    p.members.push_back({m["party"].text(), m["role"].text(), m["since"].text(), m["until"].text()});
  }
  for (std::size_t k = 0; k < j["goals"].size(); k++) {
    const Json& g = j["goals"][k];
    p.goals.push_back({g["id"].text(), g["text"].text(), strings_of(g["set_by"])});
  }
  for (std::size_t k = 0; k < j["commitments"].size(); k++) {
    const Json& c = j["commitments"][k];
    dp::Commitment x;
    x.id = c["id"].text(), x.by = c["by"].text(), x.text = c["text"].text(), x.goal = c["goal"].text(), x.made = c["made"].text(), x.due = c["due"].text();
    if (c["roles"].is_object())
      for (const auto& [party, role] : c["roles"].as_object()) x.roles[party] = role.text();
    const Json& i = c["indicator"];
    x.indicator = {i["name"].text(), i["unit"].text(), number(i["baseline"]), number(i["target"]), flag(i["higher_is_better"], true)};
    for (std::size_t q = 0; q < c["resources"].size(); q++) {
      const Json& r = c["resources"][q];
      x.resources.push_back({r["party"].text(), r["kind"].text(), r["unit"].text(), number(r["amount"], 0), flag(r["secured"])});
    }
    for (std::size_t q = 0; q < c["amendments"].size(); q++) {
      const Json& a = c["amendments"][q];
      x.amendments.push_back({a["when"].text(), a["by"].text(), a["reason"].text(), number(a["target"]), a["due"].text(), a["transferred_to"].text()});
    }
    if (c["achievable"].is_object()) {
      x.achievable_by = c["achievable"]["by"].text(), x.achievable_reason = c["achievable"]["reason"].text(), x.achievable = flag(c["achievable"]["judged"]);
    }
    p.commitments.push_back(std::move(x));
  }
  for (std::size_t k = 0; k < j["reports"].size(); k++) {
    const Json& r = j["reports"][k];
    p.reports.push_back({r["commitment"].text(), r["when"].text(), r["by"].text(), r["source"].text(), number(r["value"])});
  }
  for (std::size_t k = 0; k < j["contributions"].size(); k++) {
    const Json& c = j["contributions"][k];
    p.contributions.push_back({c["party"].text(), c["commitment"].text(), c["goal"].text(), c["kind"].text(), c["unit"].text(), c["when"].text(), c["source"].text(), number(c["amount"], 0)});
  }
  for (std::size_t k = 0; k < j["dimensions"].size(); k++) {
    const Json& d = j["dimensions"][k];
    p.dimensions.push_back({d["name"].text(), d["kind"].text(), d["unit"].text(), scale_of(d["scale"].text()), flag(d["higher_is_better"], true), strings_of(d["levels"])});
  }
  for (std::size_t k = 0; k < j["values"].size(); k++) p.values.push_back(value_of(j["values"][k]));
  for (std::size_t k = 0; k < j["interests"].size(); k++) {
    const Json& i = j["interests"][k];
    p.interests.push_back({i["party"].text(), i["dimension"].text(), i["reading"].text(), i["source"].text(), number(i["threshold"])});
  }
  return p;
}

inline Json measure_json(const kk::Measure& m) {
  const char* kind = m.kind == kk::Measure::Kind::probability ? "probability" : m.kind == kk::Measure::Kind::interval ? "interval" : m.kind == kk::Measure::Kind::estimate ? "estimate" : "none";
  return Json::object({{"kind", kind}, {"value", num(m.value)}, {"lower", num(m.lower)}, {"upper", num(m.upper)}, {"error", num(m.error)}, {"words", kk::words(m)}});
}

inline Json value_json(const dp::Value& v) {
  return Json::object({{"party", v.party}, {"dimension", v.dimension}, {"when", v.when}, {"reading", v.reading}, {"source", v.source}, {"value", num(v.value)},
                       {"lower", num(v.lo())}, {"upper", num(v.hi())}, {"error", num(v.error)}, {"grade", std::string(1, v.grade)}});
}

inline kk::Standard standard_of(const Json& s) {
  kk::Standard st;
  const std::string kind = s["kind"].text("standing");
  st.kind = kind == "probability" ? kk::Standard::Kind::probability : kind == "both" ? kk::Standard::Kind::both : kk::Standard::Kind::standing;
  st.threshold = number(s["threshold"], 0.5);
  st.least = s["least"].text("clear") == "arguable" ? kk::Standing::arguable : kk::Standing::clear;
  return st;
}

inline Json partnership(const Json& r) {
  const dp::Partnership p = partnership_of(r["partnership"]);
  const std::string at = r["at"].text();
  Json out = Json::object();
  out.set("name", p.name);
  out.set("at", at);
  out.set("problems", problems_json(dp::problems(p)));
  // the screen of each commitment, by the criteria named (all of SMART when none are)
  auto scheme = dp::smart();
  if (r["scheme"].is_array()) {
    const auto names = strings_of(r["scheme"]);
    scheme.erase(std::remove_if(scheme.begin(), scheme.end(), [&](const dp::Criterion& c) { return std::find(names.begin(), names.end(), c.name) == names.end(); }), scheme.end());
  }
  Json screens = Json::array(), states = Json::array();
  for (const auto& c : p.commitments) {
    const auto sc = dp::screen(c, scheme);
    Json checks = Json::array();
    for (const auto& k : sc.checks) checks.push(Json::object({{"name", k.name}, {"verdict", verdict_json(k.verdict)}}));
    screens.push(Json::object({{"commitment", c.id}, {"verdict", verdict_json(sc.verdict)}, {"checks", checks}}));
    const auto s = dp::state(p, c, at);
    states.push(Json::object({{"commitment", c.id}, {"text", c.text}, {"goal", c.goal}, {"by", s.by}, {"made", c.made}, {"due", s.due}, {"indicator", c.indicator.name},
                              {"unit", c.indicator.unit}, {"baseline", num(c.indicator.baseline)}, {"target", num(s.target)}, {"latest", num(s.latest)},
                              {"progress", num(s.progress)}, {"reached_on", s.reached_on}, {"last_report", s.last_report}, {"state", dp::name(s.state)},
                              {"reports", static_cast<double>(s.reports)}, {"amended", static_cast<double>(c.amendments.size())}}));
  }
  out.set("screens", screens);
  out.set("states", states);
  // contributions gathered by party, kind and unit, never across units
  std::map<std::string, std::map<std::string, double>> given;  // party → "kind|unit" → amount
  for (const auto& c : p.contributions)
    if (at.empty() || c.when <= at) given[c.party][c.kind + "|" + c.unit] += c.amount;
  Json contributions = Json::array();
  for (const auto& [party, by] : given)
    for (const auto& [ku, amount] : by) {
      const auto bar = ku.find('|');
      contributions.push(Json::object({{"party", party}, {"kind", ku.substr(0, bar)}, {"unit", ku.substr(bar + 1)}, {"amount", num(amount)}}));
    }
  out.set("contributions", contributions);
  // each party's values and benefit
  Json values = Json::object(), benefits = Json::array();
  std::set<std::string> seen;
  for (const auto& m : p.members) {
    if (!seen.insert(m.party).second) continue;
    Json vs = Json::array();
    for (const auto& [dim, v] : dp::values_of(p, m.party, at)) vs.push(value_json(v));
    values.set(m.party, vs);
    const auto f = dp::benefit(p, m.party, at);
    Json interests = Json::array();
    for (const auto& i : p.interests)
      if (i.party == m.party) interests.push(Json::object({{"dimension", i.dimension}, {"threshold", num(i.threshold)}, {"reading", i.reading}, {"source", i.source}}));
    benefits.push(Json::object({{"party", m.party}, {"standing", kk::name(f.standing)}, {"measure", measure_json(f.measure)}, {"met", strings(f.value.met)},
                                {"unmet", strings(f.value.unmet)}, {"open", strings(f.value.open)}, {"unobserved", strings(f.value.unobserved)},
                                {"family", kk::name(f.provenance.family)}, {"rests_on", strings(f.provenance.rests_on)}, {"interests", interests}}));
  }
  out.set("values", values);
  out.set("benefits", benefits);
  const auto mu = dp::mutual(p, standard_of(r["standard"]), at);
  Json checks = Json::array();
  for (const auto& c : mu.checks) checks.push(Json::object({{"party", c.name}, {"verdict", verdict_json(c.verdict)}}));
  out.set("mutual", Json::object({{"verdict", verdict_json(mu.verdict)}, {"parties", checks}}));
  dp::Thresholds th;
  if (r["thresholds"].is_object()) {
    const Json& t = r["thresholds"];
    th.inactive_days = static_cast<long>(number(t["inactive_days"], static_cast<double>(th.inactive_days)));
    th.drift = number(t["drift"], th.drift), th.fragility = number(t["fragility"], th.fragility), th.capture = number(t["capture"], th.capture);
    th.inertia_days = static_cast<long>(number(t["inertia_days"], static_cast<double>(th.inertia_days)));
  }
  Json signs = Json::array();
  if (!at.empty())
    for (const auto& s : dp::diagnose(p, at, th)) signs.push(Json::object({{"mode", s.mode}, {"where", s.where}, {"what", s.what}}));
  out.set("signs", signs);
  // alternatives compared as scales permit, and summed by a declared rule
  if (r["alternatives"].is_array()) {
    std::map<std::string, std::map<std::string, dp::Value>> alts;
    std::vector<std::string> order;
    for (std::size_t k = 0; k < r["alternatives"].size(); k++) {
      const Json& a = r["alternatives"][k];
      const std::string name = a["name"].text("alternative " + std::to_string(k + 1));
      order.push_back(name);
      if (a["values"].is_object())
        for (const auto& [dim, v] : a["values"].as_object()) {
          dp::Value x = value_of(v);
          x.dimension = dim;
          alts[name][dim] = x;
        }
    }
    Json pairs = Json::array();
    for (std::size_t i = 0; i < order.size(); i++)
      for (std::size_t j = i + 1; j < order.size(); j++) {
        const auto c = dp::compare(p.dimensions, alts[order[i]], alts[order[j]]);
        Json by = Json::object();
        for (const auto& [dim, o] : c.by) by.set(dim, dp::name(o));
        pairs.push(Json::object({{"a", order[i]}, {"b", order[j]}, {"relation", dp::name(c.relation)}, {"why", c.why}, {"by", by}}));
      }
    Json sums = Json::array();
    if (r["rule"].is_object()) {
      dp::Rule rule{r["rule"]["name"].text(), r["rule"]["unit"].text(), r["rule"]["declared_by"].text(), r["rule"]["reason"].text(), {}};
      if (r["rule"]["weights"].is_object())
        for (const auto& [dim, w] : r["rule"]["weights"].as_object()) rule.weights[dim] = number(w, 0);
      for (const auto& name : order) {
        const auto a = dp::aggregate(p.dimensions, alts[name], rule);
        sums.push(Json::object({{"alternative", name}, {"refused", a.refused}, {"value", num(a.value)}, {"lower", num(a.lower)}, {"upper", num(a.upper)}, {"unit", a.unit}, {"why", a.why}}));
      }
    }
    out.set("alternatives", Json::object({{"pairs", pairs}, {"undominated", strings(dp::undominated(p.dimensions, alts))}, {"sums", sums}}));
  }
  return out;
}

// ---- A decision rule's power ----------------------------------------------------------------------------------------------------

inline bool weighted_only(const Json& rule) { return rule["weights"].is_array() && !rule.has("all_of") && !rule.has("any_of"); }

inline ng::SimpleGame game_of(const Json& rule, const std::vector<std::string>& members) {
  const std::size_t n = members.size();
  auto index = [&](const std::string& m) {
    for (std::size_t i = 0; i < n; i++)
      if (members[i] == m) return i;
    throw std::invalid_argument("science::multilateral: " + m + " is not a member");
  };
  if (rule["weights"].is_array()) {
    ng::WeightedGame g;
    for (std::size_t k = 0; k < rule["weights"].size(); k++) g.weights.push_back(static_cast<long>(std::llround(number(rule["weights"][k], 0))));
    if (g.weights.size() != n) throw std::invalid_argument("science::multilateral: one weight per member");
    g.quota = static_cast<long>(std::llround(number(rule["quota"], 0)));
    return ng::simple(g);
  }
  if (rule["at_least"].is_number()) {
    const int k = static_cast<int>(rule["at_least"].as_number());
    return {n, [k](std::uint32_t S) { return __builtin_popcount(S) >= k; }};
  }
  if (rule["all"].is_array()) {
    std::uint32_t need = 0;
    for (const auto& m : strings_of(rule["all"])) need |= std::uint32_t{1} << index(m);
    return {n, [need](std::uint32_t S) { return (S & need) == need; }};
  }
  std::vector<ng::SimpleGame> parts;
  const Json& list = rule.has("all_of") ? rule["all_of"] : rule["any_of"];
  for (std::size_t k = 0; k < list.size(); k++) parts.push_back(game_of(list[k], members));
  if (rule.has("all_of")) return ng::all_of(parts);
  if (rule.has("any_of")) return ng::any_of(parts);
  throw std::invalid_argument("science::multilateral: a rule is {weights, quota}, {at_least}, {all}, {all_of} or {any_of}");
}

inline Json power_json(const ng::Power& p, const std::vector<std::string>& members) {
  Json a = Json::array();
  for (std::size_t i = 0; i < members.size(); i++)
    a.push(Json::object({{"member", members[i]}, {"swings", num(p.swings[i])}, {"penrose", num(p.penrose[i])}, {"banzhaf", num(p.banzhaf[i])},
                         {"shapley_shubik", num(p.shapley_shubik[i])}, {"coleman_prevent", num(p.coleman_prevent[i])}, {"coleman_initiate", num(p.coleman_initiate[i])}}));
  return a;
}

inline Json rule(const Json& r) {
  const auto members = strings_of(r["members"]);
  if (members.empty()) throw std::invalid_argument("science::multilateral: a rule needs its members");
  Json out = Json::object();
  ng::Power p;
  if (weighted_only(r["rule"]) && members.size() > 24) {
    ng::WeightedGame g;
    for (std::size_t k = 0; k < r["rule"]["weights"].size(); k++) g.weights.push_back(static_cast<long>(std::llround(number(r["rule"]["weights"][k], 0))));
    if (g.weights.size() != members.size()) throw std::invalid_argument("science::multilateral: one weight per member");
    g.quota = static_cast<long>(std::llround(number(r["rule"]["quota"], 0)));
    p = ng::power(g);
    out.set("route", "generating functions");
  } else {
    if (members.size() > 24) throw std::invalid_argument("science::multilateral: a rule other than a weighted vote is computed for at most 24 members");
    const auto g = game_of(r["rule"], members);
    p = ng::power(g);
    const auto pr = ng::properties(g);
    std::vector<std::string> vetoes, dummies;
    for (auto i : pr.vetoes) vetoes.push_back(members[i]);
    for (auto i : pr.dummies) dummies.push_back(members[i]);
    out.set("properties", Json::object({{"monotone", pr.monotone}, {"proper", pr.proper}, {"strong", pr.strong}, {"decisive", pr.decisive()}, {"vetoes", strings(vetoes)},
                                        {"dummies", strings(dummies)}, {"minimal_winning", static_cast<double>(pr.minimal_winning.size())}}));
    out.set("route", "enumeration");
  }
  out.set("power", power_json(p, members));
  out.set("power_to_act", num(p.power_to_act));
  if (r["sizes"].is_array()) {
    std::vector<double> sizes;
    for (std::size_t k = 0; k < r["sizes"].size(); k++) sizes.push_back(number(r["sizes"][k], 1));
    const auto ind = ng::indirect_power(sizes, p);
    Json a = Json::array();
    for (std::size_t i = 0; i < members.size(); i++) a.push(Json::object({{"member", members[i]}, {"size", num(sizes[i])}, {"indirect", num(ind[i])}}));
    out.set("indirect", a);
  }
  return out;
}

// ---- Ideal points ------------------------------------------------------------------------------------------------------------------

inline Json ideal_points(const Json& r) {
  const auto members = strings_of(r["members"]), items = strings_of(r["items"]);
  tyche::latent::Votes y;
  for (std::size_t i = 0; i < r["votes"].size(); i++) {
    std::vector<int> row;
    for (std::size_t j = 0; j < r["votes"][i].size(); j++) row.push_back(r["votes"][i][j].is_number() ? static_cast<int>(r["votes"][i][j].as_number()) : -1);
    y.push_back(row);
  }
  if (y.size() != members.size()) throw std::invalid_argument("science::multilateral: one row of votes per member");
  for (const auto& row : y)
    if (row.size() != items.size()) throw std::invalid_argument("science::multilateral: one vote, or none, per item in every row");
  tyche::latent::IdealPointOptions o;
  const std::string anchor = r["anchor"].text();
  for (std::size_t i = 0; i < members.size(); i++)
    if (members[i] == anchor) o.anchor = i;
  auto f = tyche::latent::ideal_points(y, o);
  const std::size_t reps = static_cast<std::size_t>(number(r["bootstrap"], 0));
  if (reps > 0) {
    calx::random::Stream rng(static_cast<std::uint64_t>(number(r["seed"], 20261119)), 132, 0, 0);
    tyche::latent::bootstrap(f, y, reps, rng, number(r["level"], 0.9), o);
  }
  const auto g = tyche::latent::standardised(f);
  Json points = Json::array(), cuts = Json::array();
  for (std::size_t i = 0; i < members.size(); i++) {
    std::size_t cast = 0;
    for (int v : y[i]) cast += (v == 0 || v == 1);
    points.push(Json::object({{"member", members[i]}, {"x", num(g.x[i])}, {"lower", g.lower.empty() ? Json(nullptr) : num(g.lower[i])},
                              {"upper", g.upper.empty() ? Json(nullptr) : num(g.upper[i])}, {"votes", static_cast<double>(cast)}}));
  }
  for (std::size_t j = 0; j < items.size(); j++)
    cuts.push(Json::object({{"item", items[j]}, {"alpha", num(g.alpha[j])}, {"beta", num(g.beta[j])}, {"cut", num(g.beta[j] != 0 ? -g.alpha[j] / g.beta[j] : NAN)}}));
  return Json::object({{"anchor", members[o.anchor]}, {"points", points}, {"items", cuts}, {"classified", num(f.classified)}, {"apre", num(f.apre)},
                       {"iterations", static_cast<double>(f.iterations)}, {"converged", f.converged}, {"log_posterior", num(f.log_posterior.back())}});
}

// ---- Bargaining, division, a joint gain -------------------------------------------------------------------------------------------

inline ng::Pair pair_of(const Json& j) { return {number(j[0], 0), number(j[1], 0)}; }
inline Json pair_json(const ng::Pair& p) { return Json::array({num(p[0]), num(p[1])}); }

inline Json bargaining(const Json& r) {
  ng::Bargaining B;
  for (std::size_t k = 0; k < r["outcomes"].size(); k++) B.outcomes.push_back(pair_of(r["outcomes"][k]));
  B.disagreement = r["disagreement"].is_array() ? pair_of(r["disagreement"]) : ng::Pair{0, 0};
  const auto n = ng::nash(B), ks = ng::kalai_smorodinsky(B), eg = ng::egalitarian(B);
  Json frontier = Json::array();
  for (const auto& p : n.frontier) frontier.push(pair_json(p));
  return Json::object({{"nash", pair_json(n.point)}, {"kalai_smorodinsky", pair_json(ks.point)}, {"egalitarian", pair_json(eg.point)}, {"ideal", pair_json(n.ideal)},
                       {"disagreement", pair_json(B.disagreement)}, {"frontier", frontier}});
}

inline Json division(const Json& r) {
  std::vector<double> a, b;
  for (std::size_t k = 0; k < r["a"].size(); k++) a.push_back(number(r["a"][k], 0));
  for (std::size_t k = 0; k < r["b"].size(); k++) b.push_back(number(r["b"][k], 0));
  const auto issues = strings_of(r["issues"]);
  const auto d = ng::adjusted_winner(a, b);
  Json shares = Json::array();
  for (std::size_t g = 0; g < d.share.size(); g++)
    shares.push(Json::object({{"issue", g < issues.size() ? issues[g] : "issue " + std::to_string(g + 1)}, {"a", num(d.share[g])}, {"b", num(1 - d.share[g])}, {"split", g == d.split}}));
  return Json::object({{"shares", shares}, {"points_a", num(d.points_a)}, {"points_b", num(d.points_b)}});
}

inline Json coalitional(const Json& r) {
  const auto members = strings_of(r["members"]);
  const std::size_t n = members.size();
  if (n == 0 || n > 10) throw std::invalid_argument("science::multilateral: a joint gain is divided among 1 to 10 members");
  std::map<std::uint32_t, double> worth;
  for (std::size_t k = 0; k < r["worths"].size(); k++) {
    std::uint32_t S = 0;
    for (const auto& m : strings_of(r["worths"][k]["coalition"])) {
      const auto it = std::find(members.begin(), members.end(), m);
      if (it == members.end()) throw std::invalid_argument("science::multilateral: " + m + " is not a member");
      S |= std::uint32_t{1} << static_cast<std::size_t>(it - members.begin());
    }
    worth[S] = number(r["worths"][k]["worth"], 0);
  }
  auto v = [&](std::uint32_t S) {
    const auto it = worth.find(S);
    return it == worth.end() ? 0.0 : it->second;
  };
  const auto phi = ng::shapley(n, v);
  const auto lc = ng::least_core(n, v);
  const auto nu = ng::prenucleolus(n, v);
  Json shares = Json::array();
  for (std::size_t i = 0; i < n; i++) shares.push(Json::object({{"member", members[i]}, {"shapley", num(phi[i])}, {"nucleolus", num(nu.x[i])}, {"least_core", num(lc.x[i])}}));
  return Json::object({{"shares", shares}, {"least_core_value", num(lc.epsilon)}, {"core_empty", !lc.core_nonempty()}, {"shapley_in_core", ng::in_core(phi, v)},
                       {"nucleolus_is_imputation", nu.imputation}, {"rounds", static_cast<double>(nu.rounds)}});
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "undertaking") return undertaking(r);
  if (op == "partnership") return partnership(r);
  if (op == "rule") return rule(r);
  if (op == "ideal_points") return ideal_points(r);
  if (op == "bargaining") return bargaining(r);
  if (op == "division") return division(r);
  if (op == "coalitional") return coalitional(r);
  if (op == "outcome") return science::affairs::outcome(r);
  if (op == "rule_game") return science::affairs::rule_game(r);
  if (op == "negotiation") return science::affairs::negotiation(r);
  if (op == "events") return science::affairs::events(r);
  if (op == "mandates") {
    if (r["undertaking"].is_object()) {
      const dr::Undertaking u = undertaking_of(r["undertaking"]);
      return science::affairs::mandates(r, &u);
    }
    return science::affairs::mandates(r, nullptr);
  }
  if (op == "reviews") return science::affairs::reviews(r);
  if (op == "institutions") return science::affairs::institutions(r);
  throw std::invalid_argument("science::multilateral: no operation is called " + op +
                              " (there is: undertaking, partnership, rule, ideal_points, bargaining, division, coalitional, outcome, rule_game, negotiation, events, "
                              "mandates, reviews, institutions)");
}

}  // namespace science::multilateral
