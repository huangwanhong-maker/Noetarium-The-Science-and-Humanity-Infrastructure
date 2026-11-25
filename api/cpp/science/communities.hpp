// Communities as one composition any work may call (ADR 0167): thesmos's constitutions, chronicle, counts and crossings, and the
// communities domain's jar, read and answered as JSON.
//
//   communities.admit   {constitution}                            the faults of form, each under its condition, and the profile
//   communities.run     {constitution, parties, acts, seals?}     each act's outcome and seal, the relational state, the head; with
//                                                                 the seals of an earlier run, whether the record replays as sealed
//   communities.count   {parties, ballots, unit, rule}            the share and its passing under the declared and the undeclared
//                                                                 relation of control
//   communities.jar     {regime, need, free_riders, epistemic, audit_cost, runs, ...}   the jar's means
//   communities.statements {constitution, texts?, parties?, acts?}  the rules as institutional statements, each duty measured
//
// A constitution is noetarium.constitution/1 (an object or its text). A party is {id, kind, controller, provider}. An act is {author,
// kind, turn, when, mandate, fields: {name: value}, holders: [[party, position]], grant: {id, principal, holder, kinds, matters,
// allowance: {resource: n}, from, to, delegable, parent, slice: {resource: n}}, charges: [{mandate, resource, amount}], body}. A
// ballot is {voter, bearer, yes}.
#pragma once

#include <communities/jar/jar.hpp>
#include <communities/presets/presets.hpp>

#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/crossings/crossings.hpp>
#include <thesmos/statements/statements.hpp>

#include <science/json.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace science::communities {

namespace ch = thesmos::chronicle;

inline const Json::Array& list(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}
inline std::string scalar(const Json& j) {
  if (j.is_string()) return j.text();
  if (j.is_number()) {
    const double x = j.as_number();
    if (x == static_cast<double>(static_cast<long long>(x))) return std::to_string(static_cast<long long>(x));
    return Json(x).dump();
  }
  if (j.is_bool()) return j.as_bool() ? "yes" : "no";
  return {};
}
inline long long whole(const Json& j, long long fallback = 0) { return j.is_number() ? static_cast<long long>(j.as_number()) : fallback; }
inline Json strings(const std::vector<std::string>& v) {
  Json out = Json::array();
  for (const auto& s : v) out.push(s);
  return out;
}
template <class Set>
inline Json strings_of(const Set& v) {
  Json out = Json::array();
  for (const auto& s : v) out.push(s);
  return out;
}

inline thesmos::constitutions::Constitution constitution_of(const Json& j) {
  if (j.is_null()) throw std::invalid_argument("communities: a request names its constitution");
  return thesmos::constitutions::read(j.is_string() ? j.text() : j.dump());
}

inline thesmos::kernel::Parties parties_of(const Json& j) {
  thesmos::kernel::Parties ps;
  for (const auto& p : list(j)) {
    const auto kind = thesmos::kernel::kind_named(p["kind"].text("person"));
    if (!kind) throw std::invalid_argument("communities: the party " + p["id"].text() + " is of no declared kind");
    ps.add({p["id"].text(), *kind, p["controller"].text(), p["provider"].text()});
  }
  return ps;
}

inline thesmos::mandates::Mandate mandate_of(const Json& g) {
  thesmos::mandates::Mandate m;
  m.id = g["id"].text(), m.principal = g["principal"].text(), m.holder = g["holder"].text(), m.parent = g["parent"].text();
  for (const auto& k : list(g["kinds"])) m.kinds.insert(k.text());
  for (const auto& k : list(g["matters"])) m.matters.insert(k.text());
  if (g["allowance"].is_object())
    for (const auto& [r, n] : g["allowance"].as_object()) m.allowance[r] = whole(n);
  if (g["slice"].is_object())
    for (const auto& [r, n] : g["slice"].as_object()) m.slice[r] = whole(n);
  m.from = whole(g["from"]), m.to = whole(g["to"]);
  m.delegable = g.has("delegable") && g["delegable"].as_bool();
  return m;
}

inline thesmos::acts::Act act_of(const Json& a) {
  thesmos::acts::Act x;
  x.author = a["author"].text(), x.kind = a["kind"].text(), x.turn = whole(a["turn"]), x.when = a["when"].text(), x.mandate = a["mandate"].text(), x.body = a["body"].text();
  if (a["fields"].is_object())
    for (const auto& [k, v] : a["fields"].as_object()) x.fields[k] = scalar(v);
  for (const auto& h : list(a["holders"])) {
    if (h.is_array() && h.as_array().size() == 2) x.holders.push_back({h.as_array()[0].text(), h.as_array()[1].text()});
    else if (h.is_object()) x.holders.push_back({h["party"].text(), h["position"].text()});
  }
  if (a["grant"].is_object()) x.grant = mandate_of(a["grant"]);
  for (const auto& c : list(a["charges"])) x.charges.push_back({c["mandate"].text(), c["resource"].text(), whole(c["amount"])});
  return x;
}

inline Json head_json(const thesmos::crossings::Head& h) { return Json::object({{"size", static_cast<double>(h.size)}, {"root", archeion::fixity::hex(h.root)}}); }

inline Json state_json(const ch::State& s) {
  Json positions = Json::array(), mandates = Json::array(), commitments = Json::array(), claims = Json::array(), proposals = Json::array();
  for (const auto& [k, since] : s.positions) positions.push(Json::object({{"party", k.first}, {"position", k.second}, {"since", static_cast<double>(since)}}));
  for (const auto& [id, m] : s.mandates) {
    Json used = Json::object();
    if (const auto u = s.used.find(id); u != s.used.end())
      for (const auto& [r, n] : u->second) used.set(r, static_cast<double>(n));
    mandates.push(Json::object({{"id", id}, {"principal", m.principal}, {"holder", m.holder}, {"revoked", s.revoked.count(id) > 0}, {"used", used}}));
  }
  for (const auto& [id, c] : s.commitments)
    commitments.push(Json::object({{"id", id}, {"debtor", c.debtor}, {"creditor", c.creditor}, {"matter", c.matter}, {"amount", static_cast<double>(c.amount)},
                                   {"status", std::string(thesmos::commitments::word(c.status))}, {"by", c.by}}));
  for (const auto& [id, c] : s.claims) claims.push(Json::object({{"id", id}, {"claimant", c.claimant}, {"against", c.against}, {"status", c.status}, {"finding", c.finding}}));
  for (const auto& [id, p] : s.proposals) {
    Json votes = Json::array(), shares = Json::object();
    for (const auto& b : p.ballots()) votes.push(Json::object({{"voter", b.voter}, {"bearer", b.bearer}, {"yes", b.yes}}));
    for (const char* unit : {"operative", "origin", "person", "community"})
      shares.set(unit, thesmos::procedures::count(s.parties, p.ballots(), *thesmos::procedures::unit_named(unit)).share().str());
    proposals.push(Json::object({{"id", id}, {"decision", p.decision}, {"status", p.status}, {"votes", votes}, {"shares", shares}}));
  }
  return Json::object({{"positions", positions}, {"mandates", mandates}, {"commitments", commitments}, {"claims", claims}, {"proposals", proposals},
                       {"compacts", strings(s.rules.compacts)}, {"assessments", static_cast<double>(s.assessments.size())}});
}

inline Json admit_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  Json faults = Json::array();
  for (const auto& f : thesmos::constitutions::admission(c)) faults.push(Json::object({{"condition", static_cast<double>(f.condition)}, {"reason", f.reason}}));
  const auto p = ::communities::presets::profile(c);
  return Json::object({{"name", c.name},
                       {"admitted", faults.as_array().empty()},
                       {"faults", faults},
                       {"profile", Json::object({{"positions", static_cast<double>(p.positions)}, {"decisions", static_cast<double>(p.decisions)}, {"counted_by", strings_of(p.counted_by)},
                                                 {"open", strings_of(p.open_positions)}, {"operatives_may_hold", strings_of(p.operative_positions)},
                                                 {"communities_may_hold", strings_of(p.community_positions)}, {"circulation", strings(p.circulation)}, {"route_for_claims", p.route_for_claims}})}});
}

inline Json run_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  const auto ps = parties_of(q["parties"]);
  ch::State s(ps, c);
  Json outcomes = Json::array(), seals = Json::array();
  std::size_t n = 0;
  for (const auto& a : list(q["acts"])) {
    const auto o = ch::step(s, act_of(a));
    n++;
    outcomes.push(Json::object({{"n", static_cast<double>(n)}, {"admitted", o.admitted}, {"reason", o.reason}, {"seal", o.admitted ? Json(s.journal.entries.back().seal) : Json()}}));
    if (o.admitted) seals.push(s.journal.entries.back().seal);
  }
  Json out = Json::object({{"outcomes", outcomes}, {"seals", seals}, {"state", state_json(s)}, {"head", head_json(thesmos::crossings::head(c.name, s))}, {"digest", ch::relational_digest(s)}});
  if (q["seals"].is_array()) {
    bool same = q["seals"].as_array().size() == s.journal.entries.size();
    std::size_t first = 0;
    for (std::size_t k = 0; same && k < s.journal.entries.size(); k++)
      if (q["seals"].as_array()[k].text() != s.journal.entries[k].seal) same = false, first = k + 1;
    out.set("replays", same);
    if (!same) out.set("differs_at", static_cast<double>(first));
  }
  return out;
}

inline Json count_op(const Json& q) {
  const auto ps = parties_of(q["parties"]);
  std::vector<thesmos::procedures::Ballot> ballots;
  for (const auto& b : list(q["ballots"])) ballots.push_back({b["voter"].text(), b["bearer"].text(b["voter"].text()), b["yes"].is_bool() && b["yes"].as_bool()});
  const auto unit = thesmos::procedures::unit_named(q["unit"].text("origin"));
  if (!unit) throw std::invalid_argument("communities: no unit of count \"" + q["unit"].text() + "\"");
  const std::string rule = q["rule"].text("majority");
  if (!thesmos::procedures::known_rule(rule)) throw std::invalid_argument("communities: no rule \"" + rule + "\"");
  thesmos::kernel::Parties bare = ps;
  for (const auto& [id, p] : ps.all()) bare.at(id).controller.clear();
  const auto declared = thesmos::procedures::count(ps, ballots, *unit), undeclared = thesmos::procedures::count(bare, ballots, *unit);
  const bool a = thesmos::procedures::passes(declared, rule), b = thesmos::procedures::passes(undeclared, rule);
  return Json::object({{"share", declared.share().str()}, {"counted", static_cast<double>(declared.counted)}, {"passes", a},
                       {"undeclared", Json::object({{"share", undeclared.share().str()}, {"counted", static_cast<double>(undeclared.counted)}, {"passes", b}})}, {"turns", a != b}});
}

inline Json jar_op(const Json& q) {
  namespace jar = ::communities::jar;
  jar::Setting s;
  const std::string regime = q["regime"].text("provision with duty");
  bool found = false;
  for (auto r : {jar::Regime::currency, jar::Regime::currency_with_income, jar::Regime::currency_with_targeted_income, jar::Regime::provision_open, jar::Regime::provision_with_duty})
    if (jar::word(r) == regime) s.regime = r, found = true;
  if (!found) throw std::invalid_argument("communities: no arrangement \"" + regime + "\"");
  if (q["need"].is_number()) s.need = whole(q["need"]);
  if (q["parties"].is_number()) s.parties = static_cast<int>(whole(q["parties"]));
  if (q["rounds"].is_number()) s.rounds = static_cast<int>(whole(q["rounds"]));
  for (const auto& [key, field] : std::vector<std::pair<const char*, double*>>{{"free_riders", &s.free_riders}, {"epistemic", &s.epistemic}, {"effort", &s.effort}, {"stock", &s.stock},
                                                                                {"income", &s.income}, {"demurrage", &s.demurrage}, {"audit_cost", &s.audit_cost}})
    if (q[key].is_number()) *field = q[key].as_number();
  if (q["tiers"].is_array()) {
    s.tiers.clear();
    for (const auto& t : q["tiers"].as_array()) s.tiers.push_back(whole(t));
  }
  const int runs = static_cast<int>(whole(q["runs"], 20));
  if (runs < 1 || runs > 400) throw std::invalid_argument("communities: between 1 and 400 runs");
  const auto r = jar::mean(s, runs);
  Json by_round = Json::array(), weakest = Json::array();
  for (double x : r.met_by_round) by_round.push(x);
  for (double x : r.weakest_by_round) weakest.push(x);
  return Json::object({{"regime", regime}, {"runs", static_cast<double>(runs)}, {"met", r.met}, {"met_weakest", r.met_weakest}, {"met_strongest", r.met_strongest},
                       {"met_free_riders", r.met_free}, {"met_cooperators", r.met_cooperators}, {"offered", r.offered}, {"idle", r.idle}, {"audits", r.audits},
                       {"refused_for_balance", r.refused_for_balance}, {"suspended", r.suspended}, {"met_by_round", by_round}, {"weakest_by_round", weakest}});
}

// The constitution's rules as institutional statements, or the statements given ({texts}); with parties and acts, each duty's and
// prohibition's facets measured over the record the constitution runs.
inline Json statements_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  std::vector<thesmos::statements::Statement> given;
  Json refused = Json::array();
  if (q["texts"].is_array())
    for (const auto& t : q["texts"].as_array()) {
      const auto r = thesmos::statements::read(t.text());
      if (r.read) given.push_back(r.statement);
      else refused.push(Json::object({{"text", t.text()}, {"error", r.error}}));
    }
  else
    given = thesmos::statements::of(c);
  ch::State s(parties_of(q["parties"]), c);
  for (const auto& a : list(q["acts"])) ch::step(s, act_of(a));
  const auto holds = [&](const std::string& party, const std::string& position) { return s.holds(party, position); };
  Json out = Json::array();
  for (const auto& st : given) {
    Json one = Json::object({{"text", st.text()}, {"attribute", st.attribute}, {"deontic", st.deontic}, {"aim", st.aim}});
    if (!s.acts.empty() && (st.deontic == "must never" || (st.deontic == "must" && !st.trigger.empty()))) {
      const auto f = thesmos::norms::facets(thesmos::statements::norm(st, holds, s.acts), s.acts);
      one.set("episodes", static_cast<double>(f.episodes));
      one.set("regularity", f.regularity ? Json(f.regularity->str()) : Json());
    }
    out.push(one);
  }
  return Json::object({{"statements", out}, {"refused", refused}});
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (op == "communities.statements") return statements_op(q);
  if (op == "communities.admit") return admit_op(q);
  if (op == "communities.run") return run_op(q);
  if (op == "communities.count") return count_op(q);
  if (op == "communities.jar") return jar_op(q);
  throw std::invalid_argument("communities: no operation \"" + op + "\"");
}

}  // namespace science::communities
