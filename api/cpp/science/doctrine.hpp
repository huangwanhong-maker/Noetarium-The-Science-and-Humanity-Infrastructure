// Theories of the law for any work's engine (ADR 0133): the law domain's doctrines (law/doctrine) composed with the matter a work
// keeps. A request holds the scheme and the matter (and, to go on with a search, its state); nothing is read from a file.
//
// What the scheme and the matter declare, beside what law.hpp reads:
//   scheme.decided     [{id, name, court, date, instrument, facts: [vocabulary ids], question, held: bool, passage, citation}]: decided
//                      cases that bear on how the open terms are read
//   scheme.precedents  [{fact, sides: [holds, fails], factors: [{id, side, text}], cases: [{name, facts, outcome, reason?, date, court,
//                      citation, passage}]}]: case bases, by factors, on facts of the vocabulary; a case's reason, unstated, is chosen
//   matter.doctrine    {situations: {fact: [factors]}, purposes: [{name, weight, worth: {term: {reading: worth}}}],
//                      weights: {authority, cases, coherence, justification, persuasive}, tolerance, held_back: [ids], search: {mu,
//                      lambda, generations, capacity, key}}
// A decision dated after the matter's date is held back (and so is one the matter lists): the search never reads it, and each
// doctrine kept is tested on it once, the test sealed in the journal.
//
//   doctrines        the search run `generations` more from `state` (begun when absent), with `proposals` [{text, source, parents}];
//                    answers the doctrines kept by kind with their assessments and tests, the questions' standings, the exact best of
//                    each kind when the doctrines are few, the population, the statistics, the journal, and the state to keep; given
//                    `requests` {budget, roles}, what to ask a model for next
//   assess_doctrine  one doctrine written by a person (`text`), read and assessed (never tested: a person sees what is held back)

#pragma once

#include "evolution_state.hpp"
#include "json.hpp"
#include "law.hpp"
#include "theories.hpp"

#include <law/doctrine/search.hpp>

#include <archeion/fixity/journal.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::doctrine {

namespace ld = ::law::doctrine;
namespace lw = ::law;
namespace lp = ::law::precedent;
namespace ai = apodeixis::interpretation;
namespace es = science::evolution_state;
namespace sl = science::law;
namespace hev = heuresis::evolution;

using es::items;
using es::num;

inline double number_or(const Json& j, const char* key, double fallback) { return j[key].is_number() ? j[key].as_number() : fallback; }
inline std::size_t count_or(const Json& j, const char* key, std::size_t fallback) { return j[key].is_number() ? static_cast<std::size_t>(j[key].as_number()) : fallback; }
inline std::vector<std::string> texts(const Json& j) {
  std::vector<std::string> v;
  for (const auto& x : items(j)) v.push_back(x.text());
  return v;
}

// ---- The problem ------------------------------------------------------------------------------------------------------------------

struct Made {
  ld::Problem p;
  Json faults = Json::array();             // what was set aside, and why
  std::map<std::string, std::string> gloss; // term → its gloss; "term=reading" → the reading's gloss
  double tolerance = 0.5;
};

inline Made problem_of(const Json& r) {
  Made out;
  const sl::Read rd = sl::scheme_of(r["scheme"]);
  const Json& sc = r["scheme"];
  const Json& m = r["matter"];
  const Json& dm = m["doctrine"];
  auto& p = out.p;
  lw::facts::Record record;
  for (const auto& x : items(m["facts"])) {
    auto f = lw::facts::fact(x["id"].text(), x["text"].text());
    f.standing = sl::standing_of(x["standing"].text("alleged"));
    record.facts.push_back(std::move(f));
  }
  for (const auto& x : items(m["issues"])) p.questions.push_back(x["question"].text());
  const auto built = sl::built(rd, m, record, p.questions);
  p.matter = built.c;
  p.forum = {m["forum"]["id"].text(), m["forum"]["name"].text("the forum"), {}};
  for (const auto& o : texts(m["forum"]["bound_by"])) p.forum.bound_by.insert(o);
  p.courts = rd.scheme.courts;
  p.date = m["date"].text();
  p.admissible = ai::admissible(p.matter.points, p.matter.arguments, p.matter.canon_order, p.matter.undercuts, p.matter.excluded).admissible;
  for (const auto& t : items(sc["terms"])) {
    out.gloss[t["name"].text()] = t["gloss"].text();
    for (const auto& x : items(t["readings"])) out.gloss[t["name"].text() + "=" + x["name"].text()] = x["gloss"].text();
  }
  std::set<std::string> listed;
  for (const auto& x : texts(dm["held_back"])) listed.insert(x);
  const auto later = [&](const std::string& date) { return !p.date.empty() && !date.empty() && date > p.date; };
  std::set<std::string> courts;
  for (const auto& c : p.courts) courts.insert(c.id);
  // Decided cases.
  for (const auto& x : items(sc["decided"])) {
    ld::Decided d;
    d.id = x["id"].text(), d.name = x["name"].text(d.id), d.court = x["court"].text(), d.date = x["date"].text(), d.instrument = x["instrument"].text();
    d.facts = texts(x["facts"]), d.question = x["question"].text(), d.held = !x["held"].is_bool() || x["held"].as_bool(), d.passage = x["passage"].text();
    d.held_back = later(d.date) || listed.count(d.id);
    if (!courts.count(d.court)) {
      out.faults.push(Json::object({{"what", "decided case " + d.id}, {"why", "its court " + d.court + " is not declared"}}));
      continue;
    }
    p.decided.push_back(std::move(d));
  }
  // Case bases.
  for (const auto& x : items(sc["precedents"])) {
    ld::CaseBase b;
    b.fact = x["fact"].text();
    const auto sides = texts(x["sides"]);
    b.base.sides = {sides.size() > 0 ? sides[0] : b.fact, sides.size() > 1 ? sides[1] : "not " + b.fact};
    for (const auto& f : items(x["factors"])) {
      std::string side = f["side"].text();
      if (side == "holds" || side == "0") side = b.base.sides[0];
      if (side == "fails" || side == "1") side = b.base.sides[1];
      b.base.side_of[f["id"].text()] = side;
      b.base.text[f["id"].text()] = f["text"].text();
    }
    for (const auto& c : items(x["cases"])) {
      lp::Precedent pc;
      pc.name = c["name"].text();
      for (const auto& f : texts(c["facts"])) pc.facts.insert(f);
      pc.outcome = c["outcome"].text();
      if (pc.outcome == "holds") pc.outcome = b.base.sides[0];
      if (pc.outcome == "fails") pc.outcome = b.base.sides[1];
      for (const auto& f : texts(c["reason"])) pc.reason.insert(f);
      if (later(c["date"].text()) || listed.count(pc.name) || listed.count(b.fact + "/" + pc.name)) b.held_back.insert(pc.name);
      b.base.cases.push_back(std::move(pc));
    }
    for (const auto& f : texts(dm["situations"][b.fact])) b.situation.insert(f);
    p.bases.push_back(std::move(b));
  }
  // Purposes: a worth for each reading of a term that reaches a question.
  for (const auto& x : items(dm["purposes"])) {
    ld::Purpose pu;
    pu.name = x["name"].text(), pu.weight = number_or(x, "weight", 1);
    if (x["worth"].is_object())
      for (const auto& [term, values] : x["worth"].as_object()) {
        const long k = ai::point_index(p.matter, term);
        if (k < 0) {
          out.faults.push(Json::object({{"what", "purpose " + pu.name}, {"why", "it values the term " + term + ", which no question of the matter reaches"}}));
          continue;
        }
        std::vector<double> w;
        for (const auto& reading : p.matter.points[static_cast<std::size_t>(k)].readings) w.push_back(values[reading].is_number() ? values[reading].as_number() : 0.0);
        pu.worth[term] = w;
      }
    p.purposes.push_back(std::move(pu));
  }
  const Json& w = dm["weights"];
  p.weights.authority = number_or(w, "authority", p.weights.authority), p.weights.cases = number_or(w, "cases", p.weights.cases);
  p.weights.coherence = number_or(w, "coherence", p.weights.coherence), p.weights.justification = number_or(w, "justification", p.weights.justification);
  p.weights.persuasive = number_or(w, "persuasive", p.weights.persuasive);
  out.tolerance = number_or(dm, "tolerance", 0.5);
  return out;
}

inline ld::SearchOptions options_of(const Json& m) {
  ld::SearchOptions o;
  const Json& q = m["doctrine"]["search"];
  o.strategy.mu = count_or(q, "mu", o.strategy.mu), o.strategy.lambda = count_or(q, "lambda", o.strategy.lambda);
  o.strategy.generations = count_or(q, "generations", 1000000);  // a work runs a search a few generations at a time
  o.capacity = count_or(q, "capacity", o.capacity), o.min_distance = number_or(q, "min_distance", o.min_distance);
  if (q["key"].is_number()) o.key = static_cast<std::uint64_t>(q["key"].as_number());
  o.tolerance = number_or(m["doctrine"], "tolerance", o.tolerance);
  return o;
}

// ---- Writing what is found ---------------------------------------------------------------------------------------------------------

inline Json doctrine_state_json(const ld::Doctrine& d) {
  Json ratios = Json::array();
  for (const auto& base : d.ratios) {
    Json b = Json::array();
    for (const auto& r : base) b.push(std::vector<std::string>(r.begin(), r.end()));
    ratios.push(b);
  }
  return Json::object({{"readings", std::vector<double>(d.readings.begin(), d.readings.end())}, {"ratios", ratios}});
}
inline ld::Doctrine doctrine_state_of(const Json& j) {
  ld::Doctrine d;
  for (const auto& x : items(j["readings"])) d.readings.push_back(static_cast<std::size_t>(x.as_number()));
  for (const auto& b : items(j["ratios"])) {
    std::vector<lp::Factors> base;
    for (const auto& r : items(b)) {
      lp::Factors f;
      for (const auto& x : items(r)) f.insert(x.text());
      base.push_back(f);
    }
    d.ratios.push_back(base);
  }
  return d;
}

inline Json assessment_json(const ld::Problem& p, const ld::Assessment& a) {
  Json cases = Json::array();
  for (const auto& c : a.cases)
    cases.push(Json::object({{"id", c.id}, {"force", std::string(lw::authority::word(c.force))}, {"weight", c.weight}, {"follows", c.follows}, {"agrees", c.agrees}, {"reason", c.reason}}));
  Json outcomes = Json::array();
  for (std::size_t q = 0; q < a.outcomes.size(); q++) outcomes.push(Json::object({{"question", p.questions[q]}, {"outcome", std::string(ld::name(a.outcomes[q]))}, {"code", static_cast<int>(a.outcomes[q])}}));
  Json bases = Json::array();
  for (std::size_t b = 0; b < a.forced.size(); b++)
    bases.push(Json::object({{"fact", p.bases[b].fact}, {"forced", a.forced[b]}, {"held", a.forced[b].empty() ? Json() : Json(a.forced[b] == p.bases[b].base.sides[0])}, {"why", a.forcing[b]}}));
  return Json::object({{"score", num(a.score)}, {"misfit", num(a.misfit)}, {"admissible", a.admissible}, {"conflicts", a.conflicts}, {"justification", num(a.justification)},
                       {"cases", cases}, {"outcomes", outcomes}, {"bases", bases}, {"summary", a.summary}});
}

inline Json doctrine_json(const ld::Problem& p, const ld::Doctrine& d, const ld::Assessment& a) {
  Json readings = Json::object(), ratios = Json::array();
  for (std::size_t k = 0; k < p.matter.points.size(); k++) readings.set(p.matter.points[k].name, p.matter.points[k].readings[d.readings[k]]);
  for (std::size_t b = 0; b < p.bases.size(); b++)
    for (std::size_t c = 0; c < p.bases[b].base.cases.size(); c++) {
      const auto& pc = p.bases[b].base.cases[c];
      if (p.bases[b].held_back.count(pc.name)) continue;
      const bool open = ld::open_ratio(p.bases[b], pc);
      const lp::Factors& r = open ? d.ratios[b][c] : pc.reason;
      ratios.push(Json::object({{"fact", p.bases[b].fact}, {"case", pc.name}, {"factors", std::vector<std::string>(r.begin(), r.end())}, {"chosen", open}}));
    }
  const auto k = ld::kind(p, a);
  return Json::object({{"text", ld::write(p, d)}, {"readings", readings}, {"ratios", ratios}, {"assessment", assessment_json(p, a)}, {"kind", k}, {"kind_words", ld::kind_words(p, k)}});
}

inline Json problem_json(const Made& made) {
  const auto& p = made.p;
  Json points = Json::array();
  for (std::size_t k = 0; k < p.matter.points.size(); k++) {
    Json rs = Json::array();
    for (std::size_t r = 0; r < p.matter.points[k].readings.size(); r++) {
      const std::string& n = p.matter.points[k].readings[r];
      const auto g = made.gloss.find(p.matter.points[k].name + "=" + n);
      rs.push(Json::object({{"name", n}, {"gloss", g == made.gloss.end() ? "" : g->second}}));
    }
    const auto g = made.gloss.find(p.matter.points[k].name);
    points.push(Json::object({{"name", p.matter.points[k].name}, {"gloss", g == made.gloss.end() ? "" : g->second}, {"readings", rs}}));
  }
  Json admissible = Json::array();
  for (std::size_t x = 0; x < p.admissible.size(); x++)
    if (p.admissible[x]) admissible.push(ai::print(p.matter.points, ai::decode(p.matter.points, x)));
  Json decided = Json::array(), held = Json::array();
  for (const auto& d : p.decided) {
    if (d.held_back) {
      held.push(Json::object({{"id", d.id}, {"name", d.name}, {"date", d.date}, {"court", d.court}}));
      continue;
    }
    lw::authority::Force f = lw::authority::Force::left_out;
    std::string why;
    const double weight = ld::force_weight(p, d, &f, &why);
    decided.push(Json::object({{"id", d.id}, {"name", d.name}, {"court", d.court}, {"date", d.date}, {"facts", d.facts}, {"question", d.question}, {"held", d.held},
                               {"force", std::string(lw::authority::word(f))}, {"weight", weight}, {"why", why}, {"passage", d.passage}}));
  }
  Json bases = Json::array();
  for (const auto& b : p.bases) {
    Json cases = Json::array(), factors = Json::array();
    for (const auto& [f, side] : b.base.side_of) {
      const auto t = b.base.text.find(f);
      factors.push(Json::object({{"id", f}, {"side", side}, {"text", t == b.base.text.end() ? "" : t->second}}));
    }
    for (const auto& c : b.base.cases) {
      if (b.held_back.count(c.name)) {
        held.push(Json::object({{"id", b.fact + "/" + c.name}, {"name", c.name}}));
        continue;
      }
      const auto fo = ld::for_outcome(b, c);
      cases.push(Json::object({{"name", c.name}, {"facts", std::vector<std::string>(c.facts.begin(), c.facts.end())}, {"outcome", c.outcome}, {"stated", !c.reason.empty()},
                               {"reason", std::vector<std::string>(c.reason.begin(), c.reason.end())}, {"for_outcome", std::vector<std::string>(fo.begin(), fo.end())}}));
    }
    bases.push(Json::object({{"fact", b.fact}, {"sides", std::vector<std::string>{b.base.sides[0], b.base.sides[1]}}, {"factors", factors}, {"cases", cases},
                             {"situation", std::vector<std::string>(b.situation.begin(), b.situation.end())}}));
  }
  Json purposes = Json::array();
  for (const auto& pu : p.purposes) {
    Json worth = Json::object();
    for (const auto& [term, w] : pu.worth) worth.set(term, w);
    purposes.push(Json::object({{"name", pu.name}, {"weight", pu.weight}, {"worth", worth}}));
  }
  return Json::object({{"points", points}, {"admissible", admissible}, {"decided", decided}, {"held_back", held}, {"bases", bases}, {"purposes", purposes},
                       {"questions", p.questions}, {"date", p.date}, {"size", num(ld::size(p))},
                       {"weights", Json::object({{"authority", p.weights.authority}, {"cases", p.weights.cases}, {"coherence", p.weights.coherence},
                                                 {"justification", p.weights.justification}, {"persuasive", p.weights.persuasive}})},
                       {"tolerance", made.tolerance}, {"faults", made.faults}});
}

inline Json tests_json(const std::vector<ld::Confirmation>& v) {
  Json out = Json::array();
  for (const auto& c : v) {
    Json cases = Json::array();
    for (const auto& x : c.cases) cases.push(Json::object({{"id", x.id}, {"verdict", x.verdict}, {"reason", x.reason}}));
    out.push(Json::object({{"ticket", static_cast<double>(c.ticket)}, {"serial", static_cast<double>(heuresis::search::serial_of(c.ticket))}, {"key", c.key}, {"text", c.text},
                           {"outcome", c.outcome}, {"cases", cases}, {"registered", c.registered}, {"written", c.written}}));
  }
  return out;
}
inline std::vector<ld::Confirmation> tests_of(const Json& j) {
  std::vector<ld::Confirmation> out;
  for (const auto& x : items(j)) {
    ld::Confirmation c;
    c.ticket = static_cast<heuresis::search::Ticket>(x["ticket"].as_number()), c.key = x["key"].text(), c.text = x["text"].text(), c.outcome = x["outcome"].text();
    c.registered = x["registered"].text(), c.written = x["written"].text();
    for (const auto& y : items(x["cases"])) c.cases.push_back({y["id"].text(), y["verdict"].text(), y["reason"].text()});
    out.push_back(std::move(c));
  }
  return out;
}

// ---- The operations ------------------------------------------------------------------------------------------------------------------

inline Json doctrines(const Json& r) {
  const Made made = problem_of(r);
  const auto& p = made.p;
  if (const auto f = ld::faults(p); !f.empty()) {
    std::string all;
    for (const auto& x : f) all += (all.empty() ? "" : "; ") + x;
    throw std::invalid_argument("the matter's law cannot be read as doctrines: " + all);
  }
  const Json& m = r["matter"];
  ld::SearchOptions o = options_of(m);
  o.when = r["now"].text("1970-01-01T00:00:00Z"), o.who = "the doctrine search";
  // The journal of tests and the tests done, verified before the search goes on.
  ld::Confirmer conf;
  for (const auto& en : items(r["state"]["journal"])) conf.journal.entries.push_back({en["when"].text(), en["who"].text(), en["what"].text(), en["about"].text(), en["note"].text(), en["seal"].text()});
  if (const auto v = archeion::fixity::verify(conf.journal); !v.holds()) throw std::invalid_argument("the search's journal does not verify: " + v.reason);
  conf.done = tests_of(r["state"]["tests"]);
  if (!conf.journal.entries.empty() && o.when < conf.journal.entries.back().when) o.when = conf.journal.entries.back().when;
  std::unique_ptr<ld::Search> s;
  if (r["state"].is_object() && r["state"]["evolution"].is_object()) s = std::make_unique<ld::Search>(p, o, es::state_of<ld::Doctrine>(r["state"]["evolution"], doctrine_state_of), conf);
  else s = std::make_unique<ld::Search>(p, o, hev::State<ld::Doctrine>{}, conf);
  Json refused = Json::array();
  for (const auto& q : items(r["proposals"])) {
    const auto rd = ld::read(p, q["text"].text());
    if (!rd.ok) {
      refused.push(Json::object({{"source", q["source"].text("a proposal")}, {"text", q["text"].text()}, {"why", rd.errors}}));
      continue;
    }
    std::vector<heuresis::search::Ticket> parents;
    for (const auto& x : items(q["parents"])) parents.push_back(static_cast<heuresis::search::Ticket>(x.as_number()));
    s->propose({rd.doctrine}, q["source"].text("a proposal"), parents.empty() ? std::vector<std::vector<heuresis::search::Ticket>>{} : std::vector<std::vector<heuresis::search::Ticket>>{parents});
  }
  s->run(count_or(r, "generations", count_or(m["doctrine"]["search"], "step", 5)));
  // The doctrines kept, by kind.
  const auto& conf_after = s->confirmer();
  std::map<std::string, const ld::Confirmation*> test_of;
  for (const auto& c : conf_after.done) test_of[c.key] = &c;
  Json kept = Json::array();
  std::vector<ld::Assessment> found;
  for (const auto& [k, a] : s->kept()) {
    Json one = doctrine_json(p, k->member.genotype, a);
    one.set("ticket", static_cast<double>(k->member.ticket));
    one.set("serial", static_cast<double>(heuresis::search::serial_of(k->member.ticket)));
    one.set("operation", k->member.operation), one.set("admitted", k->admitted), one.set("note", k->note);
    one.set("parents", std::vector<double>(k->member.parents.begin(), k->member.parents.end()));
    if (auto it = test_of.find(ld::key(p, k->member.genotype)); it != test_of.end()) one.set("test", tests_json({*it->second})[0]);
    kept.push(one);
    found.push_back(a);
  }
  Json standings = Json::array();
  for (const auto& st : ld::standings(p, found, o.tolerance))
    standings.push(Json::object({{"question", st.question}, {"verdict", std::string(ai::word(st.verdict))}, {"acceptable", st.acceptable}, {"holding", st.holding}, {"turning", st.turning},
                                 {"reason", st.reason}}));
  Json facts = Json::array();
  for (const auto& st : ld::fact_standings(p, found, o.tolerance))
    facts.push(Json::object({{"fact", st.fact}, {"verdict", st.verdict}, {"acceptable", st.acceptable}, {"held", st.held}, {"denied", st.denied}, {"open", st.open}, {"reason", st.reason}}));
  // The exact best of each kind, when the doctrines are few enough to visit every one.
  Json exact = Json::object({{"size", num(ld::size(p))}, {"enumerated", false}});
  if (ld::size(p) <= 20000) {
    Json best = Json::array();
    for (const auto& b : ld::best_of_each_kind(p, 20000))
      best.push(Json::object({{"kind", b.kind}, {"words", ld::kind_words(p, b.kind)}, {"score", num(b.assessment.score)}, {"text", ld::write(p, b.doctrine)},
                              {"assessment", assessment_json(p, b.assessment)}}));
    exact.set("enumerated", true), exact.set("best", best);
  }
  Json population = Json::array(), stats = Json::array(), retired = Json::array(), lineage = Json::array();
  {
    const auto rank = s->evolution().ranking();
    for (std::size_t k = 0; k < rank.size() && k < 16; k++) {
      const auto& mm = s->evolution().population()[rank[k]];
      population.push(Json::object({{"serial", static_cast<double>(heuresis::search::serial_of(mm.ticket))}, {"score", num(mm.evaluation.objectives.empty() ? NAN : mm.evaluation.objectives[0])},
                                    {"operation", mm.operation}, {"kind", mm.evaluation.descriptors}, {"text", ld::write(p, mm.genotype)}}));
    }
  }
  for (const auto& x : s->evolution().statistics()) stats.push(es::statistics_json(x));
  for (const auto& x : s->evolution().retired())
    retired.push(Json::object({{"serial", static_cast<double>(heuresis::search::serial_of(x.member.ticket))}, {"score", num(x.member.evaluation.objectives.empty() ? NAN : x.member.evaluation.objectives[0])},
                               {"admitted", x.admitted}, {"retired", x.retired}, {"reason", x.reason}}));
  for (const auto& l : s->evolution().lineage()) {
    Json obj = Json::array();
    for (double x : l.objectives) obj.push(num(x));
    lineage.push(Json::array({static_cast<double>(l.ticket), l.generation, l.operation, std::vector<double>(l.parents.begin(), l.parents.end()),
                              (l.failed ? 1 : 0) + (l.survived ? 2 : 0) + (l.admitted ? 4 : 0), obj}));
  }
  Json journal = Json::array();
  for (const auto& en : conf_after.journal.entries) journal.push(Json::object({{"when", en.when}, {"who", en.who}, {"what", en.what}, {"about", en.about}, {"note", en.note}, {"seal", en.seal}}));
  Json out = Json::object({{"generation", s->evolution().generation()}, {"evaluations", s->evolution().evaluations()}, {"problem", problem_json(made)}, {"kept", kept},
                           {"standings", standings}, {"fact_standings", facts}, {"exact", exact}, {"population", population}, {"statistics", stats}, {"retired", retired}, {"lineage", lineage},
                           {"operators", s->evolution().operator_names()}, {"probability", s->evolution().credits().probability}, {"tests", tests_json(conf_after.done)},
                           {"journal", journal}, {"journal_head", conf_after.journal.head()}, {"refused", refused}});
  if (r["requests"].is_object()) {
    std::vector<std::string> roles = texts(r["requests"]["roles"]);
    Json q = Json::array();
    for (const auto& x : s->requests(count_or(r["requests"], "budget", 0), roles.empty() ? ld::model_roles() : roles))
      q.push(Json::object({{"role", x.role}, {"count", x.count}, {"parents", std::vector<double>(x.parents.begin(), x.parents.end())}, {"texts", x.texts}, {"notes", x.notes}, {"credit", num(x.credit)}}));
    out.set("requests", q);
  }
  out.set("state", Json::object({{"schema", "noetarium.doctrine-search/1"}, {"evolution", es::state_json(s->evolution().state(), doctrine_state_json)}, {"journal", journal},
                                 {"tests", tests_json(conf_after.done)}}));
  return out;
}

inline Json assess_doctrine(const Json& r) {
  const Made made = problem_of(r);
  const auto rd = ld::read(made.p, r["text"].text());
  if (!rd.ok) return Json::object({{"read", false}, {"errors", rd.errors}});
  const auto a = ld::assess(made.p, rd.doctrine);
  Json one = doctrine_json(made.p, rd.doctrine, a);
  one.set("read", true);
  return one;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "doctrines") return doctrines(r);
  if (op == "assess_doctrine") return assess_doctrine(r);
  return science::theories::answer(r);
}

}  // namespace science::doctrine
