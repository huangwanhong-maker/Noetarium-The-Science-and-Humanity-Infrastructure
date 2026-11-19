// A body's decision rule as its instrument states it, and what it decides on a record of how each member acted (ADR 0134).
//
// A decision rule is written in words: "decisions … shall be made by an affirmative vote of nine members including the concurring
// votes of the permanent members" (Charter of the United Nations, Article 27(3)); "a two-thirds majority of the members present and
// voting" (Article 18(2)); "if no Member, present at the meeting when the decision is taken, formally objects" (footnote 1 to Article
// IX:1 of the Marrakesh Agreement). Each phrase is a clause, and the rule is their composition:
//
//   threshold    the acts counted, against a base, reach a fraction ("two-thirds of those present and voting", more than or at
//                least) or a number ("nine members"), the members counted one each or by a declared weight (votes, population);
//                a blocking clause is met unless the acts counted reach it ("a blocking minority of at least four members");
//   concurrence  each of some members acts in a way that concurs (the permanent members' concurring votes);
//   consensus    no member present acts in a way that blocks (a formal objection);
//   quorum       enough members are present;
//   all of, any of, fallback  ("where a decision cannot be arrived at by consensus, the matter at issue shall be decided by
//                voting", Article IX:1): the first part, and the second, on its own record, only when the first is not met.
//
// Which acts a phrase covers is often an open term. Whether a permanent member that abstains has given a concurring vote is not
// settled by the words, and the International Court of Justice found that the Council's practice treats a voluntary abstention as
// not a bar (Legal Consequences for States of the Continued Presence of South Africa in Namibia, Advisory Opinion of 21 June 1971,
// §22). Whether abstentions belong to "those present and voting" is settled for the General Assembly by its own rules (rule 86:
// members casting an affirmative or negative vote). A rule here declares such terms with the readings they admit, each reading the
// set of acts the term covers and its source (an authority, a convention, an analyst, data, a proposal), and an outcome is read under
// every combination of them by tekmerion's construal: adopted clearly when under every admissible interpretation, arguably when
// under some, untenably when under none, with the terms on which it turns. A reading that binds sets aside those beneath it.
//
// A member whose act is not recorded is not taken to have abstained or been absent: the outcome is read under every act the record's
// procedure admits for them, and when it differs among them the outcome is said to turn on the members not recorded.
//
// What follows from a record, under one interpretation:
//   - the tally of each clause in words, with whether it is met;
//   - the members whose act, changed alone, would have changed the outcome (decisive, ex post);
//   - the simple game the rule makes when every member either acts one way or another (nomos's games), whose a priori power is a
//     measure of the rule, never of influence.
//
// A rule whose thresholds count only affirmative votes, whose concurrences admit them and whose consensus is not blocked by them is
// monotone: a member who changes to yes never defeats a question the record carried (the theory note, Proposition 7.2).
//
// Nothing here is any body's rule until a caller states it; the checks state real rules' words on invented members and records.
#pragma once

#include <apodeixis/kernel/verdict.hpp>
#include <nomos/games/power.hpp>
#include <tekmerion/construal/construal.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::rules {

using apodeixis::kernel::Verdict;
namespace tc = tekmerion::construal;
namespace ap = apodeixis::interpretation;

// ---- Acts ------------------------------------------------------------------------------------------------------------------------

enum class Act { yes, no, abstain, not_voting, absent, objection, reservation };

inline const std::vector<Act>& every_act() {
  static const std::vector<Act> all{Act::yes, Act::no, Act::abstain, Act::not_voting, Act::absent, Act::objection, Act::reservation};
  return all;
}

// The act's name in a record: yes, no, abstain, not voting, absent, objection, reservation.
inline const char* name(Act a) {
  switch (a) {
    case Act::yes: return "yes";
    case Act::no: return "no";
    case Act::abstain: return "abstain";
    case Act::not_voting: return "not voting";
    case Act::absent: return "absent";
    case Act::objection: return "objection";
    default: return "reservation";
  }
}

// The act in words, as done by a member.
inline const char* words(Act a) {
  switch (a) {
    case Act::yes: return "voted yes or joined";
    case Act::no: return "voted no";
    case Act::abstain: return "abstained";
    case Act::not_voting: return "was present and did not vote";
    case Act::absent: return "was absent";
    case Act::objection: return "objected formally";
    default: return "joined with a reservation";
  }
}

inline Act act_of(const std::string& s) {
  for (Act a : every_act())
    if (s == name(a)) return a;
  throw std::invalid_argument("diplomacy::rules: no act is called " + s + " (yes, no, abstain, not voting, absent, objection, reservation)");
}

using Acts = std::set<Act>;

inline Acts all_acts() { return Acts(every_act().begin(), every_act().end()); }
inline Acts present() { return {Act::yes, Act::no, Act::abstain, Act::not_voting, Act::objection, Act::reservation}; }
inline Acts present_and_voting() { return {Act::yes, Act::no}; }

inline std::string listed(const Acts& acts) {
  std::string s;
  for (Act a : every_act())
    if (acts.count(a)) s += (s.empty() ? "" : ", ") + std::string(name(a));
  return s.empty() ? "no act" : s;
}

// ---- The rule --------------------------------------------------------------------------------------------------------------------

// A reading of an open term: the acts it takes the term to cover, and where the reading comes from.
struct Reading {
  std::string name;
  Acts acts;
  tc::Source source = tc::Source::analyst;
  char grade = 'A';
  std::string by, rationale;
  bool stated = false;  // the reading the rule's author takes as written
};

struct Term {
  std::string name, about;
  std::vector<Reading> readings;
};

// The acts a role of a clause covers: fixed, or those an open term covers under its reading.
struct Role {
  std::string term;
  Acts acts;
};

struct Clause {
  enum class Kind { threshold, concurrence, consensus, quorum, all_of, any_of, fallback };
  Kind kind = Kind::threshold;
  std::string id, text;
  std::string stage;                       // the record the clause reads (a consensus, then a vote); empty: the question's only record
  Role counted{{}, {Act::yes}};            // threshold: the acts counted; concurrence: those that concur; consensus: those that block;
                                           // quorum: those that make a member present
  Role base{{}, all_acts()};               // threshold: the acts that make up the base of a fraction
  std::vector<std::string> members;        // the members the clause reads (empty: every member of the body)
  std::string weights;                     // a declared weight by member (empty: each member one)
  long long num = 1, den = 2;              // the fraction num/den of the base …
  bool strict = true;                      // … to be exceeded (more than) or reached (at least)
  long long count = 0;                     // when above 0: a number to reach instead of a fraction ("nine members")
  bool against = false;                    // blocking: met unless the acts counted reach the threshold
  std::vector<Clause> parts;               // all of, any of; fallback: the first, then the second when the first is not met
};

inline const char* name(Clause::Kind k) {
  switch (k) {
    case Clause::Kind::threshold: return "threshold";
    case Clause::Kind::concurrence: return "concurrence";
    case Clause::Kind::consensus: return "consensus";
    case Clause::Kind::quorum: return "quorum";
    case Clause::Kind::all_of: return "all of";
    case Clause::Kind::any_of: return "any of";
    default: return "fallback";
  }
}

inline Clause::Kind kind_of(const std::string& s) {
  for (auto k : {Clause::Kind::threshold, Clause::Kind::concurrence, Clause::Kind::consensus, Clause::Kind::quorum, Clause::Kind::all_of,
                 Clause::Kind::any_of, Clause::Kind::fallback})
    if (s == name(k)) return k;
  throw std::invalid_argument("diplomacy::rules: no kind of clause is called " + s);
}

struct Rule {
  std::string id, title, instrument, text;  // the body's words, and the instrument and article where they stand
  std::vector<std::string> members;
  std::map<std::string, std::map<std::string, long long>> weights;  // name → member → weight
  std::vector<Term> terms;
  Clause clause;
  const Term* term(const std::string& n) const {
    for (const auto& t : terms)
      if (t.name == n) return &t;
    return nullptr;
  }
};

// How each member acted at one stage of a question; a member not listed is not recorded.
struct Record {
  enum class Procedure { vote, consensus };
  Procedure procedure = Procedure::vote;
  std::map<std::string, Act> acts;
};

// The acts a procedure admits for a member whose act is not recorded.
inline std::vector<Act> admitted(Record::Procedure p) {
  if (p == Record::Procedure::consensus) return {Act::yes, Act::objection, Act::reservation, Act::absent};
  return {Act::yes, Act::no, Act::abstain, Act::absent};
}

struct Question {
  std::string id, label, when;
  std::map<std::string, Record> records;  // by stage; "" when there is one
  std::string recorded;                   // what the body's record says: "adopted", "not adopted", or empty
};

struct Problem {
  std::string where, what;
};

namespace detail {

inline void roles_of(const Clause& c, std::vector<const Role*>& out) {
  out.push_back(&c.counted), out.push_back(&c.base);
  for (const auto& p : c.parts) roles_of(p, out);
}

inline void stages_of(const Clause& c, std::set<std::string>& out) {
  if (c.kind != Clause::Kind::all_of && c.kind != Clause::Kind::any_of && c.kind != Clause::Kind::fallback) out.insert(c.stage);
  for (const auto& p : c.parts) stages_of(p, out);
}

inline void clause_problems(const Rule& r, const Clause& c, const std::string& at, std::vector<Problem>& out) {
  const std::string where = at + (c.id.empty() ? "" : " " + c.id);
  const std::set<std::string> body(r.members.begin(), r.members.end());
  for (const auto& m : c.members)
    if (!body.count(m)) out.push_back({where, m + " is not a member of the body"});
  if (!c.weights.empty()) {
    auto w = r.weights.find(c.weights);
    if (w == r.weights.end()) out.push_back({where, "it counts by " + c.weights + ", which is not declared"});
    else
      for (const auto& m : c.members.empty() ? r.members : c.members) {
        auto x = w->second.find(m);
        if (x == w->second.end()) out.push_back({where, m + " has no weight in " + c.weights});
        else if (x->second < 0) out.push_back({where, m + "'s weight in " + c.weights + " is negative"});
      }
  }
  for (const Role* role : {&c.counted, &c.base})
    if (!role->term.empty() && !r.term(role->term)) out.push_back({where, "it names the term " + role->term + ", which is not declared"});
  switch (c.kind) {
    case Clause::Kind::threshold:
    case Clause::Kind::quorum:
      if (c.count < 0) out.push_back({where, "a number to reach is not negative"});
      if (c.count == 0 && (c.den <= 0 || c.num < 0 || c.num > c.den)) out.push_back({where, "a fraction num/den with 0 ≤ num ≤ den and den above 0"});
      break;
    case Clause::Kind::concurrence:
      if (c.members.empty()) out.push_back({where, "a concurrence names the members whose concurrence it requires"});
      break;
    case Clause::Kind::all_of:
    case Clause::Kind::any_of:
      if (c.parts.empty()) out.push_back({where, "it composes no clause"});
      break;
    case Clause::Kind::fallback:
      if (c.parts.size() != 2) out.push_back({where, "a fallback has two parts: the first, and what decides when it is not met"});
      break;
    default: break;
  }
  for (std::size_t k = 0; k < c.parts.size(); k++) clause_problems(r, c.parts[k], where + "/" + std::to_string(k + 1), out);
}

}  // namespace detail

// Every way a rule's declaration fails, each named.
inline std::vector<Problem> problems(const Rule& r) {
  std::vector<Problem> out;
  std::set<std::string> seen;
  if (r.members.empty()) out.push_back({"rule " + r.id, "it has no members"});
  for (const auto& m : r.members)
    if (!seen.insert(m).second) out.push_back({"rule " + r.id, m + " is a member twice"});
  std::set<std::string> terms;
  for (const auto& t : r.terms) {
    if (!terms.insert(t.name).second) out.push_back({"term " + t.name, "declared twice"});
    if (t.readings.empty()) out.push_back({"term " + t.name, "it has no reading"});
    std::size_t stated = 0;
    for (const auto& x : t.readings) stated += x.stated;
    if (stated > 1) out.push_back({"term " + t.name, "more than one reading is stated"});
  }
  detail::clause_problems(r, r.clause, "clause", out);
  return out;
}

// Every way a question's records fail against the rule, each named.
inline std::vector<Problem> problems(const Rule& r, const Question& q) {
  std::vector<Problem> out;
  const std::set<std::string> body(r.members.begin(), r.members.end());
  std::set<std::string> stages;
  detail::stages_of(r.clause, stages);
  for (const auto& [stage, rec] : q.records) {
    const std::string where = "question " + q.id + (stage.empty() ? "" : ", " + stage);
    if (!stages.count(stage)) out.push_back({where, "the rule reads no record called " + (stage.empty() ? std::string("the record") : stage)});
    for (const auto& [m, a] : rec.acts) {
      if (!body.count(m)) out.push_back({where, m + " is not a member of the body"});
      if (rec.procedure == Record::Procedure::consensus && (a == Act::no || a == Act::abstain || a == Act::not_voting))
        out.push_back({where, m + " " + words(a) + ", which a consensus does not admit"});
      if (rec.procedure == Record::Procedure::vote && (a == Act::objection || a == Act::reservation))
        out.push_back({where, m + " " + words(a) + ", which a vote does not admit"});
    }
  }
  if (!q.recorded.empty() && q.recorded != "adopted" && q.recorded != "not adopted")
    out.push_back({"question " + q.id, "its recorded outcome is adopted or not adopted"});
  return out;
}

// ---- Evaluation under one interpretation -----------------------------------------------------------------------------------------

// What each open term covers under an interpretation.
using Meaning = std::map<std::string, Acts>;

// The acts of every member at every stage, the record completed.
using Acts_by_stage = std::map<std::string, std::map<std::string, Act>>;

struct Tally {
  std::string clause, kind, words;
  bool met = false;
  std::vector<Tally> parts;
};

namespace detail {

inline const Acts& covers(const Role& role, const Meaning& meaning) {
  if (role.term.empty()) return role.acts;
  auto it = meaning.find(role.term);
  if (it == meaning.end()) throw std::invalid_argument("diplomacy::rules: the term " + role.term + " has no reading");
  return it->second;
}

inline std::string fraction(long long num, long long den) { return std::to_string(num) + "/" + std::to_string(den); }

inline Tally evaluate(const Rule& r, const Clause& c, const Meaning& meaning, const Acts_by_stage& acts) {
  Tally t;
  t.clause = c.id.empty() ? std::string(name(c.kind)) : c.id;
  t.kind = name(c.kind);
  if (c.kind == Clause::Kind::all_of || c.kind == Clause::Kind::any_of) {
    bool all = true, any = false;
    for (const auto& p : c.parts) {
      t.parts.push_back(evaluate(r, p, meaning, acts));
      all = all && t.parts.back().met, any = any || t.parts.back().met;
    }
    t.met = c.kind == Clause::Kind::all_of ? all : any;
    std::size_t met = 0;
    for (const auto& p : t.parts) met += p.met;
    t.words = std::to_string(met) + " of its " + std::to_string(t.parts.size()) + " parts met; " + (c.kind == Clause::Kind::all_of ? "all are needed" : "one suffices");
    return t;
  }
  if (c.kind == Clause::Kind::fallback) {
    t.parts.push_back(evaluate(r, c.parts[0], meaning, acts));
    if (t.parts[0].met) return t.met = true, t.words = "the first part is met, and decides", t;
    if (!acts.count(c.parts[1].stage) && c.parts[1].kind != Clause::Kind::all_of && c.parts[1].kind != Clause::Kind::any_of)
      return t.met = false, t.words = "the first part is not met, and nothing is recorded of what was to decide then", t;
    t.parts.push_back(evaluate(r, c.parts[1], meaning, acts));
    t.met = t.parts[1].met;
    t.words = std::string("the first part is not met, and the second decides: ") + (t.met ? "met" : "not met");
    return t;
  }
  auto stage = acts.find(c.stage);
  if (stage == acts.end()) return t.met = false, t.words = "nothing is recorded at " + (c.stage.empty() ? std::string("this question") : c.stage), t;
  const auto& who = c.members.empty() ? r.members : c.members;
  const std::map<std::string, long long>* w = nullptr;
  if (!c.weights.empty()) w = &r.weights.at(c.weights);
  auto weight = [&](const std::string& m) { return w ? w->at(m) : 1LL; };
  auto act = [&](const std::string& m) {
    auto it = stage->second.find(m);
    if (it == stage->second.end()) throw std::invalid_argument("diplomacy::rules: " + m + "'s act is missing from a completed record");
    return it->second;
  };
  const Acts& counted = covers(c.counted, meaning);
  const std::string unit = c.weights.empty() ? "members" : c.weights;
  switch (c.kind) {
    case Clause::Kind::concurrence: {
      std::vector<std::string> not_concurring;
      for (const auto& m : who)
        if (!counted.count(act(m))) not_concurring.push_back(m + " " + words(act(m)));
      t.met = not_concurring.empty();
      if (t.met) t.words = "every member whose concurrence it requires concurs (" + listed(counted) + " concur)";
      else {
        t.words = "not concurring, as " + listed(counted) + " concur: ";
        for (std::size_t k = 0; k < not_concurring.size(); k++) t.words += (k ? "; " : "") + not_concurring[k];
      }
      return t;
    }
    case Clause::Kind::consensus: {
      std::vector<std::string> blocking;
      for (const auto& m : who)
        if (counted.count(act(m))) blocking.push_back(m + " " + words(act(m)));
      t.met = blocking.empty();
      if (t.met) t.words = "no member blocks it (" + listed(counted) + " would)";
      else {
        t.words = "blocked: ";
        for (std::size_t k = 0; k < blocking.size(); k++) t.words += (k ? "; " : "") + blocking[k];
      }
      return t;
    }
    case Clause::Kind::quorum:
    case Clause::Kind::threshold: {
      const Acts& base = c.kind == Clause::Kind::quorum ? all_acts() : covers(c.base, meaning);
      long long C = 0, B = 0;
      for (const auto& m : who) {
        const Act a = act(m);
        if (!base.count(a)) continue;
        B += weight(m);
        if (counted.count(a)) C += weight(m);
      }
      bool reach;
      std::string need;
      if (c.count > 0) {
        reach = C >= c.count;
        need = "at least " + std::to_string(c.count);
      } else if (B == 0) {
        reach = false;
        need = (c.strict ? "more than " : "at least ") + fraction(c.num, c.den) + " of none";
      } else {
        reach = c.strict ? C * c.den > c.num * B : C * c.den >= c.num * B;
        need = (c.strict ? "more than " : "at least ") + fraction(c.num, c.den) + " of " + std::to_string(B);
      }
      t.met = c.against ? !reach : reach;
      const std::string what = c.kind == Clause::Kind::quorum ? "present" : listed(counted);
      t.words = std::to_string(C) + " " + unit + " " + what + (c.count > 0 ? "" : " of " + std::to_string(B) + " in its base (" + listed(base) + ")") + ", " +
                need + (c.against ? " would block it: " : " needed: ") + (reach ? "reached" : "not reached");
      return t;
    }
    default: return t;
  }
}

}  // namespace detail

// The outcome of the rule on records whose every member's act is known.
inline Tally evaluate(const Rule& r, const Meaning& meaning, const Acts_by_stage& acts) { return detail::evaluate(r, r.clause, meaning, acts); }

// The meaning an interpretation gives the rule's terms.
inline Meaning meaning_of(const Rule& r, const ap::Interpretation& i) {
  Meaning m;
  for (std::size_t k = 0; k < r.terms.size(); k++) m[r.terms[k].name] = r.terms[k].readings.at(i.at(k)).acts;
  return m;
}

// The stated interpretation (each term at its stated reading, else its first).
inline ap::Interpretation stated(const Rule& r) {
  ap::Interpretation i(r.terms.size(), 0);
  for (std::size_t k = 0; k < r.terms.size(); k++)
    for (std::size_t x = 0; x < r.terms[k].readings.size(); x++)
      if (r.terms[k].readings[x].stated) i[k] = x;
  return i;
}

// ---- A question's outcome under every reading -----------------------------------------------------------------------------------

enum class Answer { adopted, not_adopted, turns_on_readings, turns_on_unrecorded, undecided };

inline const char* name(Answer a) {
  switch (a) {
    case Answer::adopted: return "adopted";
    case Answer::not_adopted: return "not adopted";
    case Answer::turns_on_readings: return "turns on the readings";
    case Answer::turns_on_unrecorded: return "turns on the members not recorded";
    default: return "undecided";
  }
}

struct Decisive {
  std::string member;
  Act act;                 // as recorded
  std::vector<Act> flips;  // the other acts the procedure admits that, alone, would have changed the outcome
};

struct Outcome {
  std::string question;
  Answer answer = Answer::undecided;
  std::string reason;
  tc::Construal construal;                       // "adopted" read under the rule's terms
  std::vector<std::string> unrecorded;           // by stage: "stage: member"
  std::vector<char> depends;                     // per interpretation: whether the outcome turns on the members not recorded
  Tally tally;                                   // under the stated interpretation, the record completed as far as it is
  bool tally_complete = false;
  std::vector<Decisive> decisive;                // under the stated interpretation, when the record is complete
  std::string discrepancy;                       // when the body's record says otherwise than the rule
};

struct Options {
  std::size_t completions = 4096;  // ways the members not recorded may have acted, read at most
  tc::Options construal{};
};

namespace detail {

struct Missing {
  std::string stage, member;
  std::vector<Act> acts;
};

// Calls f on every completion of the records, until f returns false.
inline void complete(Acts_by_stage acts, const std::vector<Missing>& missing, std::size_t k, const std::function<bool(const Acts_by_stage&)>& f, bool& go) {
  if (!go) return;
  if (k == missing.size()) {
    go = f(acts);
    return;
  }
  for (Act a : missing[k].acts) {
    acts[missing[k].stage][missing[k].member] = a;
    complete(acts, missing, k + 1, f, go);
    if (!go) return;
  }
}

}  // namespace detail

inline Outcome outcome(const Rule& r, const Question& q, const Options& o = {}) {
  if (const auto ps = problems(r); !ps.empty()) throw std::invalid_argument("diplomacy::rules: rule " + r.id + ": " + ps[0].where + ": " + ps[0].what);
  if (const auto ps = problems(r, q); !ps.empty()) throw std::invalid_argument("diplomacy::rules: " + ps[0].where + ": " + ps[0].what);
  Outcome out;
  out.question = q.id;
  std::set<std::string> stages;
  detail::stages_of(r.clause, stages);
  Acts_by_stage known;
  std::vector<detail::Missing> missing;
  std::size_t ways = 1;
  bool too_many = false;
  for (const auto& stage : stages) {
    auto rec = q.records.find(stage);
    if (rec == q.records.end()) continue;  // nothing recorded at this stage: a clause there is not met
    known[stage] = rec->second.acts;
    for (const auto& m : r.members)
      if (!rec->second.acts.count(m)) {
        missing.push_back({stage, m, admitted(rec->second.procedure)});
        out.unrecorded.push_back((stage.empty() ? "" : stage + ": ") + m);
        ways *= missing.back().acts.size();
        if (ways > o.completions) too_many = true;
      }
  }
  if (too_many) {
    out.answer = Answer::undecided;
    out.reason = std::to_string(out.unrecorded.size()) + " members' acts are not recorded, too many to read every way they may have acted";
    return out;
  }
  // The judge: adopted under every completion of the records. Whether it turns on the members not recorded is kept apart.
  std::map<ap::Interpretation, char> turns;  // per interpretation, whether it turns on the members not recorded
  auto judge = [&](const ap::Interpretation& i) {
    const Meaning m = meaning_of(r, i);
    bool any_yes = false, any_no = false;
    bool go = true;
    detail::complete(known, missing, 0, [&](const Acts_by_stage& a) {
      (evaluate(r, m, a).met ? any_yes : any_no) = true;
      return !(any_yes && any_no);
    }, go);
    turns[i] = any_yes && any_no;
    if (any_yes && !any_no) return Verdict::yes("adopted", missing.empty() ? "adopted" : "adopted under every act the members not recorded may have taken");
    if (any_no && !any_yes) return Verdict::no("adopted", missing.empty() ? "not adopted" : "not adopted, whatever the members not recorded did");
    return Verdict::unknown("adopted", "it turns on the members not recorded");
  };
  tc::Finding f;
  f.claim = "question " + q.id + " is adopted under " + (r.title.empty() ? "rule " + r.id : r.title);
  for (const auto& t : r.terms) {
    tc::Point p{t.name, t.about, {}};
    for (const auto& x : t.readings) p.readings.push_back({x.name, x.source, x.grade, x.by, x.rationale, x.stated});
    f.points.push_back(std::move(p));
  }
  f.judge = judge;
  out.construal = tc::construe(f, o.construal);
  const std::size_t n = out.construal.holds.size();
  out.depends.assign(n, 0);
  bool any_adopted = false, any_rejected = false, any_depends = false, any_admissible = false;
  for (std::size_t x = 0; x < n; x++) {
    const bool dep = turns[ap::decode(out.construal.points, x)] != 0;
    out.depends[x] = dep;
    if (!out.construal.admissible[x]) continue;
    any_admissible = true;
    if (dep) any_depends = true;
    else if (out.construal.holds[x]) any_adopted = true;
    else any_rejected = true;
  }
  if (!any_admissible) out.answer = Answer::undecided, out.reason = "no interpretation of the rule's terms is admissible";
  else if (any_depends) out.answer = Answer::turns_on_unrecorded, out.reason = "under some admissible reading the outcome turns on how the members not recorded acted";
  else if (any_adopted && any_rejected) out.answer = Answer::turns_on_readings, out.reason = "adopted under some admissible readings of the rule's terms and not under others";
  else if (any_adopted) out.answer = Answer::adopted, out.reason = "adopted under every admissible reading of the rule's terms";
  else out.answer = Answer::not_adopted, out.reason = "not adopted under any admissible reading of the rule's terms";
  // The tally and the decisive members under the stated interpretation.
  const Meaning m = meaning_of(r, stated(r));
  if (missing.empty()) {
    out.tally = evaluate(r, m, known), out.tally_complete = true;
    const bool adopted = out.tally.met;
    for (const auto& stage : stages) {
      auto rec = q.records.find(stage);
      if (rec == q.records.end()) continue;
      for (const auto& [member, act] : rec->second.acts) {
        Decisive d{member, act, {}};
        for (Act alt : admitted(rec->second.procedure)) {
          if (alt == act) continue;
          auto changed = known;
          changed[stage][member] = alt;
          if (evaluate(r, m, changed).met != adopted) d.flips.push_back(alt);
        }
        if (!d.flips.empty()) out.decisive.push_back(std::move(d));
      }
    }
  }
  if (!q.recorded.empty()) {
    if (q.recorded == "adopted" && out.answer == Answer::not_adopted)
      out.discrepancy = "recorded as adopted, but under no admissible reading of the rule does the record carry it";
    else if (q.recorded == "not adopted" && out.answer == Answer::adopted)
      out.discrepancy = "recorded as not adopted, but under every admissible reading of the rule the record carries it";
    else if (q.recorded == "adopted" && out.answer == Answer::turns_on_readings)
      out.discrepancy = "recorded as adopted, which only some admissible readings of the rule's terms carry";
  }
  return out;
}

// ---- The rule as a simple game ----------------------------------------------------------------------------------------------------

// The simple game the rule makes under an interpretation when every member of a coalition acts `in` and every other member `out`,
// at every stage: the coalition wins when the rule adopts. Its a priori power (nomos::games::power) is the rule's alone.
// At a stage that a consensus reads, members out of the coalition object formally.
inline nomos::games::SimpleGame game(const Rule& r, const Meaning& meaning, Act in = Act::yes, Act out = Act::no) {
  if (const auto ps = problems(r); !ps.empty()) throw std::invalid_argument("diplomacy::rules: rule " + r.id + ": " + ps[0].where + ": " + ps[0].what);
  if (r.members.size() > 24) throw std::invalid_argument("diplomacy::rules: a rule's game is enumerated for at most 24 members");
  std::set<std::string> stages, consensus;
  detail::stages_of(r.clause, stages);
  std::function<void(const Clause&)> find = [&](const Clause& c) {
    if (c.kind == Clause::Kind::consensus) consensus.insert(c.stage);
    for (const auto& p : c.parts) find(p);
  };
  find(r.clause);
  return {r.members.size(), [r, meaning, in, out, stages, consensus](std::uint32_t S) {
            Acts_by_stage acts;
            for (const auto& stage : stages) {
              const bool c = consensus.count(stage) > 0;
              for (std::size_t k = 0; k < r.members.size(); k++) acts[stage][r.members[k]] = (S >> k & 1u) ? (c ? Act::yes : in) : (c ? Act::objection : out);
            }
            return evaluate(r, meaning, acts).met;
          }};
}

}  // namespace diplomacy::rules
