// The affairs of an undertaking for any work's engine (ADR 0134): one composition of the diplomacy domain's rules, negotiation,
// events, mandates, reviews and institutions, answering requests in JSON. A request holds everything it needs; nothing is read from a
// file or kept between requests. science::multilateral::answer dispatches here.
//
//   outcome       {rule, question}   a question's outcome under a rule as its instrument states it (diplomacy::rules): the answer
//                 (adopted, not adopted, turns on the readings, turns on the members not recorded), every interpretation of the rule's
//                 open terms with what it gives and whether it is admissible, the readings set aside, the pivots, the tally under the
//                 stated readings, the members decisive ex post, and a recorded outcome the rule does not carry
//   rule_game     {rule}   the a priori power of the simple game the rule makes under its stated readings (at most 24 members)
//   negotiation   {negotiation, claimed?}   the zone of every draft and of every option, the drafts in order (what each settles and
//                 reopens, the notes), linkage, each party's win-set, and the value claimed between two parties
//   events        {events, vocabulary?, channels?, parties?, criterion?, pairs?, by_year?}   each event's support, weighed when
//                 channels are given; coincident records; signs; each pair's series
//   mandates      {registry, at, undertaking?}   each mandate's state, lineage and review; overlaps; loads; signs
//   reviews       {record, at, mild?, answer_within?}   each recommendation's state, repeats, measures by party reviewed and cycle,
//                 signs
//   institutions  {landscape, at, issues?, transfers?, windows?}   the lattice of memberships and its regularities, each issue's
//                 complex, design, and reciprocity over windows of the given days
//
// A rule, noetarium.rule/1:
//   {id, title, instrument, text, members: [...], weights: {name: {member: w}},
//    terms: [{name, about, readings: [{name, acts: [...], source, grade, by, rationale, stated}]}],
//    clause: {kind, id, text, stage, counted, base, members, weights, fraction: [p, q], strict, count, against, parts: [clause, …]}}
// where counted and base are a list of acts, {"term": name}, or one of "members", "present", "present and voting"; acts are yes,
// no, abstain, not voting, absent, objection, reservation. A question: {id, label, when, recorded,
//   records: {stage: {procedure: "vote" | "consensus", acts: {member: act}}}} (or acts and procedure directly, for one record).
//
// A negotiation: {name, issues: [{id, text, options: [{id, text}]}],
//   parties: [{id, name, valuation, ratifiers: [{id, name, valuation}], ratification: rule}],
//   drafts: [{id, when, text, open: {issue: [option, …]}}]}
// with a valuation {values: {issue: {option: v | [lo, hi]}}, none: {issue: v | [lo, hi]}, outside: v | [lo, hi]}.
//
// Events: [{id, category, mode, context, actors, targets, date, until, place, reports: [{id, source, origin, published, passage, says}]}];
// a vocabulary {name, source, categories: [{name, meaning, cooperation, material}]}; channels {sources: {name: [hit, false alarm]},
// origins: {…}, relay: [h, f], origin: [h, f]}.
//
// A registry of mandates: {mandates: [{id, title, instrument, paragraph, issued_by, issued, addressees, themes, begins, ends,
// review_every, review_by, resourced, resources, earlier: [{link, mandate}]}], reviews: [{mandate, date, by, finding}]}.
//
// A record of reviews: {cycles: [{id, opened, closed}], recommendations: [{id, cycle, from, to, text, theme, level, repeats}],
// answers: [{recommendation, date, by, response}], progress: [{recommendation, date, by, done, evidence}]}.
//
// A landscape: {parties, institutions: [{id, name, kind, issues, design: {dimension: text}}], memberships: [{party, institution,
// since, until}]}; transfers [{from, to, date, value}].
//
// The methods are the domain's and the foundations'; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <diplomacy/events/events.hpp>
#include <diplomacy/institutions/institutions.hpp>
#include <diplomacy/mandates/mandates.hpp>
#include <diplomacy/negotiation/negotiation.hpp>
#include <diplomacy/relations/relations.hpp>
#include <diplomacy/reviews/reviews.hpp>
#include <diplomacy/rules/rules.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::affairs {

namespace drl = ::diplomacy::rules;
namespace dng = ::diplomacy::negotiation;
namespace dev = ::diplomacy::events;
namespace dmd = ::diplomacy::mandates;
namespace drv = ::diplomacy::reviews;
namespace din = ::diplomacy::institutions;

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
template <class P>
Json problems_json(const std::vector<P>& ps) {
  Json a = Json::array();
  for (const auto& p : ps) a.push(Json::object({{"where", p.where}, {"what", p.what}}));
  return a;
}
template <class S>
Json signs_json(const std::vector<S>& ss) {
  Json a = Json::array();
  for (const auto& x : ss) a.push(Json::object({{"kind", x.kind}, {"where", x.where}, {"what", x.what}}));
  return a;
}
inline std::string standing(kybernesis::kernel::Standing s) { return kybernesis::kernel::name(s); }

// ---- Rules ----------------------------------------------------------------------------------------------------------------------

inline drl::Acts acts_of(const Json& j) {
  drl::Acts a;
  for (const auto& s : strings_of(j)) a.insert(drl::act_of(s));
  return a;
}

inline drl::Role role_of(const Json& j, const drl::Role& fallback) {
  if (j.is_null()) return fallback;
  if (j.is_object()) return {j["term"].text(), {}};
  if (j.is_string()) {
    const std::string s = j.as_string();
    if (s == "members") return {{}, drl::all_acts()};
    if (s == "present") return {{}, drl::present()};
    if (s == "present and voting") return {{}, drl::present_and_voting()};
    return {{}, {drl::act_of(s)}};
  }
  return {{}, acts_of(j)};
}

inline drl::Clause clause_of(const Json& j) {
  drl::Clause c;
  c.kind = drl::kind_of(j["kind"].text("threshold"));
  c.id = j["id"].text(), c.text = j["text"].text(), c.stage = j["stage"].text();
  drl::Role counted = c.counted;
  if (c.kind == drl::Clause::Kind::consensus) counted = {{}, {drl::Act::objection}};
  if (c.kind == drl::Clause::Kind::quorum) counted = {{}, drl::present()};
  c.counted = role_of(j["counted"], counted);
  c.base = role_of(j["base"], c.base);
  c.members = strings_of(j["members"]);
  c.weights = j["weights"].text();
  if (j["fraction"].size() == 2) c.num = static_cast<long long>(std::llround(number(j["fraction"][0], 1))), c.den = static_cast<long long>(std::llround(number(j["fraction"][1], 2)));
  c.strict = flag(j["strict"], true);
  c.count = static_cast<long long>(std::llround(number(j["count"], 0)));
  c.against = flag(j["against"]);
  for (std::size_t k = 0; k < j["parts"].size(); k++) c.parts.push_back(clause_of(j["parts"][k]));
  return c;
}

inline drl::Rule rule_of(const Json& j) {
  drl::Rule r;
  r.id = j["id"].text("rule"), r.title = j["title"].text(), r.instrument = j["instrument"].text(), r.text = j["text"].text();
  r.members = strings_of(j["members"]);
  if (j["weights"].is_object())
    for (const auto& [name, w] : j["weights"].as_object())
      if (w.is_object())
        for (const auto& [m, x] : w.as_object()) r.weights[name][m] = static_cast<long long>(std::llround(number(x, 0)));
  for (std::size_t k = 0; k < j["terms"].size(); k++) {
    const Json& t = j["terms"][k];
    drl::Term term{t["name"].text(), t["about"].text(), {}};
    for (std::size_t x = 0; x < t["readings"].size(); x++) {
      const Json& rd = t["readings"][x];
      const std::string grade = rd["grade"].text("A");
      term.readings.push_back({rd["name"].text(), acts_of(rd["acts"]), tekmerion::construal::source_of(rd["source"].text("analyst")), grade.empty() ? 'A' : grade[0],
                               rd["by"].text(), rd["rationale"].text(), flag(rd["stated"])});
    }
    r.terms.push_back(std::move(term));
  }
  r.clause = clause_of(j["clause"]);
  return r;
}

inline drl::Record record_of(const Json& j) {
  drl::Record rec;
  rec.procedure = j["procedure"].text("vote") == "consensus" ? drl::Record::Procedure::consensus : drl::Record::Procedure::vote;
  if (j["acts"].is_object())
    for (const auto& [m, a] : j["acts"].as_object()) rec.acts[m] = drl::act_of(a.text());
  return rec;
}

inline drl::Question question_of(const Json& j) {
  drl::Question q;
  q.id = j["id"].text("question"), q.label = j["label"].text(), q.when = j["when"].text(), q.recorded = j["recorded"].text();
  if (j["records"].is_object())
    for (const auto& [stage, rec] : j["records"].as_object()) q.records[stage] = record_of(rec);
  else if (j.has("acts"))
    q.records[""] = record_of(j);
  return q;
}

inline Json tally_json(const drl::Tally& t) {
  Json parts = Json::array();
  for (const auto& p : t.parts) parts.push(tally_json(p));
  return Json::object({{"clause", t.clause}, {"kind", t.kind}, {"met", t.met}, {"words", t.words}, {"parts", parts}});
}

inline Json outcome(const Json& r) {
  const drl::Rule rule = rule_of(r["rule"]);
  const drl::Question q = question_of(r["question"]);
  const drl::Outcome o = drl::outcome(rule, q);
  Json interpretations = Json::array();
  for (std::size_t x = 0; x < o.construal.holds.size(); x++) {
    const auto i = apodeixis::interpretation::decode(o.construal.points, x);
    Json readings = Json::object();
    for (std::size_t k = 0; k < rule.terms.size(); k++) readings.set(rule.terms[k].name, rule.terms[k].readings[i[k]].name);
    const std::string gives = !o.depends.empty() && o.depends[x] ? "turns on the members not recorded" : o.construal.holds[x] ? "adopted" : "not adopted";
    interpretations.push(Json::object({{"readings", readings}, {"gives", gives}, {"admissible", o.construal.admissible[x] != 0}, {"reason", o.construal.reasons[x]}}));
  }
  Json pivots = Json::array();
  for (const auto& p : o.construal.pivots) {
    if (p.flips.empty()) continue;
    Json flips = Json::array();
    for (std::size_t k = 0; k < p.flips.size(); k++)
      flips.push(Json::object({{"reading", rule.terms[p.point].readings[p.flips[k]].name}, {"admissible", p.admissible[k] != 0}}));
    pivots.push(Json::object({{"term", rule.terms[p.point].name}, {"flips", flips}}));
  }
  Json decisive = Json::array();
  for (const auto& d : o.decisive) {
    Json flips = Json::array();
    for (auto a : d.flips) flips.push(std::string(drl::name(a)));
    decisive.push(Json::object({{"member", d.member}, {"act", std::string(drl::name(d.act))}, {"flips", flips}}));
  }
  Json out = Json::object({{"question", o.question}, {"answer", std::string(drl::name(o.answer))}, {"reason", o.reason},
                           {"interpretations", interpretations}, {"set_aside", strings(o.construal.set_aside)}, {"pivots", pivots},
                           {"unrecorded", strings(o.unrecorded)}, {"decisive", decisive}, {"discrepancy", o.discrepancy}});
  out.set("tally", o.tally_complete ? tally_json(o.tally) : Json(nullptr));
  return out;
}

inline Json rule_game(const Json& r) {
  const drl::Rule rule = rule_of(r["rule"]);
  const auto g = drl::game(rule, drl::meaning_of(rule, drl::stated(rule)));
  const auto p = nomos::games::power(g);
  const auto props = nomos::games::properties(g);
  Json members = Json::array();
  for (std::size_t k = 0; k < rule.members.size(); k++)
    members.push(Json::object({{"member", rule.members[k]}, {"penrose", num(p.penrose[k])}, {"banzhaf", num(p.banzhaf[k])}, {"shapley_shubik", num(p.shapley_shubik[k])},
                               {"veto", std::find(props.vetoes.begin(), props.vetoes.end(), k) != props.vetoes.end()},
                               {"dummy", std::find(props.dummies.begin(), props.dummies.end(), k) != props.dummies.end()}}));
  return Json::object({{"members", members}, {"power_to_act", num(p.power_to_act)}, {"proper", props.proper}, {"strong", props.strong}, {"monotone", props.monotone}});
}

// ---- Negotiation ------------------------------------------------------------------------------------------------------------------

inline dng::Interval interval_of(const Json& j, dng::Interval fallback = {0, 0}) {
  if (j.is_number()) return {j.as_number(), j.as_number()};
  if (j.size() == 2) return {number(j[0], 0), number(j[1], 0)};
  return fallback;
}

inline dng::Valuation valuation_of(const Json& j) {
  dng::Valuation v;
  if (j["values"].is_object())
    for (const auto& [issue, opts] : j["values"].as_object())
      if (opts.is_object())
        for (const auto& [o, x] : opts.as_object()) v.values[issue][o] = interval_of(x);
  if (j["none"].is_object())
    for (const auto& [issue, x] : j["none"].as_object()) v.none[issue] = interval_of(x);
  v.outside = interval_of(j["outside"]);
  return v;
}

inline dng::Negotiation negotiation_of(const Json& j) {
  dng::Negotiation n;
  n.name = j["name"].text();
  for (std::size_t k = 0; k < j["issues"].size(); k++) {
    const Json& is = j["issues"][k];
    dng::Issue issue{is["id"].text(), is["text"].text(), {}};
    for (std::size_t o = 0; o < is["options"].size(); o++) issue.options.push_back({is["options"][o]["id"].text(), is["options"][o]["text"].text()});
    n.issues.push_back(std::move(issue));
  }
  for (std::size_t k = 0; k < j["parties"].size(); k++) {
    const Json& p = j["parties"][k];
    dng::Party party{p["id"].text(), p["name"].text(), valuation_of(p["valuation"]), {}, {}};
    for (std::size_t x = 0; x < p["ratifiers"].size(); x++)
      party.ratifiers.push_back({p["ratifiers"][x]["id"].text(), p["ratifiers"][x]["name"].text(), valuation_of(p["ratifiers"][x]["valuation"])});
    if (p["ratification"].is_object()) party.ratification = rule_of(p["ratification"]);
    n.parties.push_back(std::move(party));
  }
  for (std::size_t k = 0; k < j["drafts"].size(); k++) {
    const Json& d = j["drafts"][k];
    dng::Draft draft{d["id"].text(), d["when"].text(), d["text"].text(), {}};
    if (d["open"].is_object())
      for (const auto& [issue, opts] : d["open"].as_object()) draft.open[issue] = strings_of(opts);
    n.drafts.push_back(std::move(draft));
  }
  return n;
}

inline Json interval_json(const dng::Interval& x) { return Json::array({num(x.lo), num(x.hi)}); }

inline Json zone_json(const dng::Negotiation& n, const dng::Zone& z, std::size_t most = 200) {
  Json listed = Json::array();
  for (std::size_t k = 0; k < z.agreeable.size() && k < most; k++) {
    const auto& r = z.agreeable[k];
    Json choice = Json::object(), parties = Json::array();
    for (std::size_t i = 0; i < r.choice.size(); i++) choice.set(n.issues[i].id, n.issues[i].options[r.choice[i]].id);
    for (std::size_t j = 0; j < r.parties.size(); j++)
      parties.push(Json::object({{"party", r.parties[j].party}, {"standing", standing(r.parties[j].standing)}, {"why", r.parties[j].why},
                                 {"value", n.parties[j].valuation.declared() ? interval_json(r.values[j]) : Json(nullptr)}}));
    listed.push(Json::object({{"choice", choice}, {"words", dng::words(n, r.choice)}, {"agreement", standing(r.agreement)}, {"parties", parties}}));
  }
  Json undominated = Json::array();
  for (std::size_t k : z.undominated)
    if (k < most) undominated.push(static_cast<double>(k));
  return Json::object({{"draft", z.draft}, {"packages", static_cast<double>(z.packages)}, {"clear", static_cast<double>(z.clear)}, {"arguable", static_cast<double>(z.arguable)},
                       {"agreeable", listed}, {"listed_all", z.agreeable.size() <= most}, {"undominated", undominated}, {"dominance_read", z.dominance_read},
                       {"settled", strings(z.settled)}, {"blocking", strings(z.blocking)}});
}

inline Json negotiation(const Json& r) {
  const dng::Negotiation n = negotiation_of(r["negotiation"]);
  Json out = Json::object({{"problems", problems_json(dng::problems(n))}});
  if (!dng::problems(n).empty()) return out;
  const dng::Zone all = dng::zone(n);
  out.set("every_option", zone_json(n, all));
  Json steps = Json::array();
  for (const auto& s : dng::trajectory(n))
    steps.push(Json::object({{"draft", s.draft}, {"when", s.when}, {"zone", zone_json(n, s.zone)}, {"settled_now", strings(s.settled_now)}, {"reopened", strings(s.reopened)}, {"notes", strings(s.notes)}}));
  out.set("drafts", steps);
  const dng::Linkage l = dng::linkage(n);
  Json alone = Json::array();
  for (const auto& a : l.alone) alone.push(Json::object({{"issue", a.issue}, {"agreeable", static_cast<double>(a.agreeable)}, {"clear", static_cast<double>(a.clear)}}));
  out.set("linkage", Json::object({{"alone", alone}, {"linked", static_cast<double>(l.linked)}, {"linked_clear", static_cast<double>(l.linked_clear)}, {"created", l.created}}));
  // Each party's win-set over every option: the packages it accepts, clearly or arguably.
  Json wins = Json::array();
  std::vector<std::size_t> every(n.issues.size());
  for (std::size_t i = 0; i < every.size(); i++) every[i] = i;
  for (const auto& party : n.parties) {
    std::size_t clear = 0, arguable = 0, count = 1;
    for (const auto& is : n.issues) count *= is.options.size();
    if (count <= 200'000) {
      const auto table = party.ratifiers.empty() ? std::vector<char>{} : dng::ratifying(party);
      dng::Package p(n.issues.size(), 0);
      for (std::size_t c = 0; c < count; c++) {
        const auto a = dng::answer(n, party, p, every, party.ratifiers.empty() ? nullptr : &table);
        clear += a.standing == kybernesis::kernel::Standing::clear, arguable += a.standing == kybernesis::kernel::Standing::arguable;
        for (std::size_t k = p.size(); k-- > 0;) {
          if (++p[k] < n.issues[k].options.size()) break;
          p[k] = 0;
        }
      }
    }
    wins.push(Json::object({{"party", party.id}, {"clear", static_cast<double>(clear)}, {"arguable", static_cast<double>(arguable)}, {"of", static_cast<double>(count)}}));
  }
  out.set("win_sets", wins);
  std::vector<std::string> pair = strings_of(r["claimed"]);
  if (pair.empty()) {
    for (const auto& p : n.parties)
      if (p.valuation.declared() && pair.size() < 2) pair.push_back(p.id);
  }
  if (pair.size() == 2) {
    try {
      const auto c = dng::claimed(n, all, pair[0], pair[1]);
      auto sol = [&](const nomos::games::BargainingSolution& s, std::size_t at) {
        return Json::object({{"point", Json::array({num(s.point[0]), num(s.point[1])})}, {"package", dng::words(n, all.agreeable[at].choice)}});
      };
      Json frontier = Json::array();
      for (const auto& f : c.nash.frontier) frontier.push(Json::array({num(f[0]), num(f[1])}));
      out.set("claimed", Json::object({{"between", strings(pair)}, {"basis", c.basis}, {"nash", sol(c.nash, c.at_nash)}, {"kalai_smorodinsky", sol(c.kalai_smorodinsky, c.at_kalai_smorodinsky)},
                                       {"egalitarian", sol(c.egalitarian, c.at_egalitarian)}, {"frontier", frontier}}));
    } catch (const std::invalid_argument& e) {
      out.set("claimed", Json::object({{"between", strings(pair)}, {"refused", std::string(e.what())}}));
    }
  }
  return out;
}

// ---- Events ------------------------------------------------------------------------------------------------------------------------

inline std::vector<dev::Event> events_of(const Json& j) {
  std::vector<dev::Event> out;
  for (std::size_t k = 0; k < j.size(); k++) {
    const Json& e = j[k];
    dev::Event x;
    x.id = e["id"].text(), x.category = e["category"].text(), x.mode = e["mode"].text(), x.context = e["context"].text();
    x.actors = strings_of(e["actors"]), x.targets = strings_of(e["targets"]);
    x.date = e["date"].text(), x.until = e["until"].text(), x.place = e["place"].text();
    for (std::size_t r = 0; r < e["reports"].size(); r++) {
      const Json& rp = e["reports"][r];
      x.reports.push_back({rp["id"].text(), rp["source"].text(), rp["origin"].text(), rp["published"].text(), rp["passage"].text(), flag(rp["says"], true)});
    }
    out.push_back(std::move(x));
  }
  return out;
}

inline tekmerion::evidence::Channel channel_of(const Json& j, tekmerion::evidence::Channel fallback) {
  if (j.size() == 2) return {number(j[0], fallback.hit), number(j[1], fallback.false_alarm)};
  return fallback;
}

inline Json events(const Json& r) {
  const auto es = events_of(r["events"]);
  dev::Vocabulary v = dev::plover();
  if (r["vocabulary"].is_object()) {
    v = {r["vocabulary"]["name"].text("declared"), r["vocabulary"]["source"].text(), {}};
    for (std::size_t k = 0; k < r["vocabulary"]["categories"].size(); k++) {
      const Json& c = r["vocabulary"]["categories"][k];
      v.categories.push_back({c["name"].text(), c["meaning"].text(), flag(c["cooperation"], true), flag(c["material"])});
    }
  }
  Json out = Json::object({{"problems", problems_json(dev::problems(es, v, strings_of(r["parties"])))}});
  Json vocab = Json::array();
  for (const auto& c : v.categories) vocab.push(Json::object({{"name", c.name}, {"meaning", c.meaning}, {"class", dev::class_of(c)}}));
  out.set("vocabulary", Json::object({{"name", v.name}, {"source", v.source}, {"categories", vocab}}));
  dev::Channels ch;
  const bool weigh = r["channels"].is_object();
  if (weigh) {
    const Json& c = r["channels"];
    if (c["sources"].is_object())
      for (const auto& [name, x] : c["sources"].as_object()) ch.sources[name] = channel_of(x, ch.relay);
    if (c["origins"].is_object())
      for (const auto& [name, x] : c["origins"].as_object()) ch.origins[name] = channel_of(x, ch.origin);
    ch.relay = channel_of(c["relay"], ch.relay), ch.origin = channel_of(c["origin"], ch.origin);
  }
  Json each = Json::array();
  for (const auto& e : es) {
    const auto s = dev::support(e, weigh ? &ch : nullptr);
    const dev::Category* c = v.category(e.category);
    Json item = Json::object({{"id", e.id}, {"category", e.category}, {"class", c ? dev::class_of(*c) : std::string()}, {"date", e.date},
                              {"actors", strings(e.actors)}, {"targets", strings(e.targets)}, {"reports", static_cast<double>(s.reports)},
                              {"sources", static_cast<double>(s.sources)}, {"origins", static_cast<double>(s.origins)}, {"denials", static_cast<double>(s.denials)},
                              {"origin_names", strings(s.origin_names)}});
    if (s.weighed) item.set("ratio", num(s.ratio)), item.set("as_independent", num(s.as_independent)), item.set("overstatement", num(s.overstatement));
    each.push(item);
  }
  out.set("events", each);
  dev::Criterion crit;
  if (r["criterion"].is_object()) crit = {static_cast<long>(number(r["criterion"]["window"], 3)), flag(r["criterion"]["same_category"], true), flag(r["criterion"]["shared_target"], true)};
  Json co = Json::array();
  for (const auto& c : dev::coincident(es, crit)) co.push(Json::object({{"a", c.a}, {"b", c.b}, {"what", c.what}}));
  out.set("coincident", co);
  out.set("signs", signs_json(dev::signs(es)));
  std::vector<std::pair<std::string, std::string>> pairs;
  for (std::size_t k = 0; k < r["pairs"].size(); k++) pairs.push_back({r["pairs"][k][0].text(), r["pairs"][k][1].text()});
  if (pairs.empty()) {
    std::set<std::pair<std::string, std::string>> seen;
    for (const auto& e : es)
      for (const auto& a : e.actors)
        for (const auto& b : e.targets)
          if (a != b) seen.insert(a < b ? std::make_pair(a, b) : std::make_pair(b, a));
    pairs.assign(seen.begin(), seen.end());
  }
  Json series = Json::array();
  for (const auto& [a, b] : pairs) {
    Json periods = Json::array();
    for (const auto& p : dev::series(es, a, b, flag(r["by_year"]), v)) {
      Json ev = Json::object(), rp = Json::object();
      for (const auto& [k, x] : p.events) ev.set(k, static_cast<double>(x));
      for (const auto& [k, x] : p.reports) rp.set(k, static_cast<double>(x));
      periods.push(Json::object({{"period", p.period}, {"events", ev}, {"reports", rp}, {"net", static_cast<double>(p.net)}}));
    }
    series.push(Json::object({{"pair", Json::array({a, b})}, {"periods", periods}}));
  }
  out.set("series", series);
  return out;
}

// ---- Mandates ---------------------------------------------------------------------------------------------------------------------

inline dmd::Registry registry_of(const Json& j) {
  dmd::Registry r;
  for (std::size_t k = 0; k < j["mandates"].size(); k++) {
    const Json& m = j["mandates"][k];
    dmd::Mandate x;
    x.id = m["id"].text(), x.title = m["title"].text(), x.instrument = m["instrument"].text(), x.paragraph = m["paragraph"].text();
    x.issued_by = m["issued_by"].text(), x.issued = m["issued"].text();
    x.addressees = strings_of(m["addressees"]), x.themes = strings_of(m["themes"]);
    x.begins = m["begins"].text(), x.ends = m["ends"].text();
    x.review_every = static_cast<long>(number(m["review_every"], 0)), x.review_by = m["review_by"].text();
    x.resourced = flag(m["resourced"]), x.resources = m["resources"].text();
    for (std::size_t e = 0; e < m["earlier"].size(); e++) x.earlier.push_back({dmd::link_of(m["earlier"][e]["link"].text("renews")), m["earlier"][e]["mandate"].text()});
    r.mandates.push_back(std::move(x));
  }
  for (std::size_t k = 0; k < j["reviews"].size(); k++) {
    const Json& v = j["reviews"][k];
    r.reviews.push_back({v["mandate"].text(), v["date"].text(), v["by"].text(), v["finding"].text()});
  }
  return r;
}

// With an undertaking (read by science::multilateral), a mandate's authority and addressees are read against it.
inline Json mandates(const Json& r, const ::diplomacy::relations::Undertaking* u) {
  const dmd::Registry reg = registry_of(r["registry"]);
  const std::string at = r["at"].text();
  Json out = Json::object({{"problems", problems_json(dmd::problems(reg))}});
  if (!dmd::problems(reg).empty()) return out;
  Json each = Json::array();
  for (const auto& m : reg.mandates) {
    const auto s = dmd::status(reg, m.id, at);
    const auto l = dmd::lineage(reg, m.id);
    const auto d = dmd::due(reg, m.id, at);
    Json earlier = Json::array();
    for (const auto& p : m.earlier) earlier.push(Json::object({{"link", std::string(dmd::name(p.link))}, {"mandate", p.mandate}}));
    each.push(Json::object({{"id", m.id}, {"title", m.title}, {"instrument", m.instrument}, {"paragraph", m.paragraph}, {"issued_by", m.issued_by},
                            {"addressees", strings(m.addressees)}, {"themes", strings(m.themes)}, {"begins", m.begins}, {"ends", m.ends},
                            {"resourced", m.resourced}, {"resources", m.resources}, {"earlier", earlier},
                            {"state", std::string(dmd::name(s.state))}, {"by", s.by}, {"why", s.why},
                            {"lineage", Json::object({{"before", strings(l.before)}, {"after", strings(l.after)}, {"roots", strings(l.roots)},
                                                      {"themes_added", strings(l.themes_added)}, {"addressees_added", strings(l.addressees_added)}})},
                            {"review", Json::object({{"last", d.last}, {"due", d.due}, {"overdue", d.overdue}, {"never", d.never}})}}));
  }
  out.set("mandates", each);
  out.set("in_force", strings(dmd::in_force(reg, at)));
  Json ov = Json::array();
  for (const auto& o : dmd::overlaps(reg, at)) ov.push(Json::object({{"a", o.a}, {"b", o.b}, {"addressees", strings(o.addressees)}, {"themes", strings(o.themes)}}));
  out.set("overlaps", ov);
  Json loads = Json::array();
  for (const auto& l : dmd::loads(reg, at)) loads.push(Json::object({{"addressee", l.addressee}, {"mandates", static_cast<double>(l.mandates)}, {"resourced", static_cast<double>(l.resourced)}}));
  out.set("loads", loads);
  out.set("signs", signs_json(dmd::signs(reg, at, u)));
  return out;
}

// ---- Reviews ------------------------------------------------------------------------------------------------------------------------

inline drv::Record reviews_of(const Json& j) {
  drv::Record r;
  for (std::size_t k = 0; k < j["cycles"].size(); k++) r.cycles.push_back({j["cycles"][k]["id"].text(), j["cycles"][k]["opened"].text(), j["cycles"][k]["closed"].text()});
  for (std::size_t k = 0; k < j["recommendations"].size(); k++) {
    const Json& x = j["recommendations"][k];
    r.recommendations.push_back({x["id"].text(), x["cycle"].text(), x["from"].text(), x["to"].text(), x["text"].text(), x["theme"].text(),
                                 static_cast<int>(number(x["level"], 0)), x["repeats"].text()});
  }
  for (std::size_t k = 0; k < j["answers"].size(); k++) {
    const Json& a = j["answers"][k];
    r.answers.push_back({a["recommendation"].text(), a["date"].text(), a["by"].text(), drv::response_of(a["response"].text("no response"))});
  }
  for (std::size_t k = 0; k < j["progress"].size(); k++) {
    const Json& p = j["progress"][k];
    r.progress.push_back({p["recommendation"].text(), p["date"].text(), p["by"].text(), drv::done_of(p["done"].text("not implemented")), strings_of(p["evidence"])});
  }
  return r;
}

inline Json reviews(const Json& r) {
  const drv::Record rec = reviews_of(r["record"]);
  const std::string at = r["at"].text();
  Json out = Json::object({{"problems", problems_json(drv::problems(rec))}});
  if (!drv::problems(rec).empty()) return out;
  Json each = Json::array();
  for (const auto& x : rec.recommendations) {
    const auto s = drv::state(rec, x.id, at);
    each.push(Json::object({{"id", x.id}, {"cycle", x.cycle}, {"from", x.from}, {"to", x.to}, {"text", x.text}, {"theme", x.theme}, {"level", static_cast<double>(x.level)},
                            {"response", std::string(drv::name(s.response))}, {"reported", s.reported}, {"done", s.reported ? std::string(drv::name(s.done)) : std::string()},
                            {"standing", std::string(drv::name(s.standing))}, {"words", s.words}}));
  }
  out.set("recommendations", each);
  Json reps = Json::array();
  for (const auto& x : drv::repeats(rec)) reps.push(Json::object({{"later", x.later}, {"earlier", x.earlier}, {"declared", x.declared}}));
  out.set("repeats", reps);
  std::set<std::pair<std::string, std::string>> groups;
  for (const auto& x : rec.recommendations) groups.insert({x.to, x.cycle});
  Json ms = Json::array();
  for (const auto& [to, cycle] : groups) {
    const auto m = drv::measures(rec, to, cycle, at);
    Json byr = Json::object(), bys = Json::object();
    for (const auto& [k, v] : m.by_response) byr.set(k, static_cast<double>(v));
    for (const auto& [k, v] : m.by_state) bys.set(k, static_cast<double>(v));
    ms.push(Json::object({{"reviewed", to}, {"cycle", cycle}, {"recommendations", static_cast<double>(m.recommendations)}, {"by_response", byr}, {"by_state", bys},
                          {"share_supported", num(m.share_supported)}}));
  }
  out.set("measures", ms);
  out.set("signs", signs_json(drv::signs(rec, at, {static_cast<long>(number(r["answer_within"], 90)), static_cast<int>(number(r["mild"], 0))})));
  return out;
}

// ---- Institutions -----------------------------------------------------------------------------------------------------------------

inline din::Landscape landscape_of(const Json& j) {
  din::Landscape l;
  l.parties = strings_of(j["parties"]);
  for (std::size_t k = 0; k < j["institutions"].size(); k++) {
    const Json& i = j["institutions"][k];
    din::Institution x{i["id"].text(), i["name"].text(), i["kind"].text(), strings_of(i["issues"]), {}};
    if (i["design"].is_object())
      for (const auto& [d, t] : i["design"].as_object()) x.design[d] = t.text();
    l.institutions.push_back(std::move(x));
  }
  for (std::size_t k = 0; k < j["memberships"].size(); k++) {
    const Json& m = j["memberships"][k];
    l.memberships.push_back({m["party"].text(), m["institution"].text(), m["since"].text(), m["until"].text()});
  }
  return l;
}

inline Json institutions(const Json& r) {
  const din::Landscape l = landscape_of(r["landscape"]);
  const std::string at = r["at"].text();
  Json out = Json::object({{"problems", problems_json(din::problems(l))}});
  if (!din::problems(l).empty()) return out;
  const auto L = din::lattice(l, at);
  Json groups = Json::array(), covers = Json::array();
  for (const auto& g : L.groups) groups.push(Json::object({{"parties", strings(g.parties)}, {"institutions", strings(g.institutions)}}));
  for (const auto& [a, b] : L.covers) covers.push(Json::array({static_cast<double>(a), static_cast<double>(b)}));
  out.set("lattice", Json::object({{"groups", groups}, {"covers", covers}, {"top", static_cast<double>(L.top)}, {"bottom", static_cast<double>(L.bottom)}}));
  Json regs = Json::array();
  for (const auto& x : din::regularities(l, at)) regs.push(Json::object({{"premise", strings(x.premise)}, {"conclusion", strings(x.conclusion)}}));
  out.set("regularities", regs);
  std::vector<std::string> issues = strings_of(r["issues"]);
  if (issues.empty()) {
    std::set<std::string> all;
    for (const auto& i : l.institutions) all.insert(i.issues.begin(), i.issues.end());
    issues.assign(all.begin(), all.end());
  }
  Json complexes = Json::array();
  for (const auto& issue : issues) {
    const auto c = din::complex(l, issue, at);
    Json pairs = Json::array(), forums = Json::array();
    for (const auto& p : c.pairs) pairs.push(Json::object({{"a", p.a}, {"b", p.b}, {"relation", std::string(din::name(p.relation))}, {"inner", p.inner}, {"shared", static_cast<double>(p.shared)}}));
    for (const auto& [party, in] : c.forums) forums.push(Json::object({{"party", party}, {"institutions", strings(in)}}));
    complexes.push(Json::object({{"issue", issue}, {"institutions", strings(c.institutions)}, {"pairs", pairs}, {"forums", forums}}));
  }
  out.set("complexes", complexes);
  Json members = Json::array();
  for (const auto& i : l.institutions) {
    Json design = Json::object(), who = Json::array();
    for (const auto& [d, t] : i.design) design.set(d, t);
    for (const auto& p : l.parties)
      if (din::member(l, p, i.id, at)) who.push(p);
    members.push(Json::object({{"id", i.id}, {"name", i.name}, {"kind", i.kind}, {"issues", strings(i.issues)}, {"design", design}, {"members", who}}));
  }
  out.set("institutions", members);
  if (r["transfers"].size() > 0) {
    std::vector<din::Transfer> ts;
    for (std::size_t k = 0; k < r["transfers"].size(); k++) {
      const Json& t = r["transfers"][k];
      ts.push_back({t["from"].text(), t["to"].text(), t["date"].text(), number(t["value"], 0)});
    }
    std::vector<long> windows;
    for (std::size_t k = 0; k < r["windows"].size(); k++) windows.push_back(static_cast<long>(number(r["windows"][k], 30)));
    if (windows.empty()) windows = {30, 91, 365};
    Json rec = Json::array();
    for (long w : windows) {
      const auto x = din::reciprocity(ts, w);
      Json ws = Json::array();
      for (const auto& win : x.windows) ws.push(Json::object({{"from", win.from}, {"until", win.until}, {"total", num(win.total)}, {"pairwise", num(win.pairwise)}, {"group", num(win.group)}}));
      rec.push(Json::object({{"days", static_cast<double>(w)}, {"pairwise", num(x.pairwise)}, {"group", num(x.group)}, {"windows", ws}}));
    }
    out.set("reciprocity", rec);
  }
  return out;
}

}  // namespace science::affairs
