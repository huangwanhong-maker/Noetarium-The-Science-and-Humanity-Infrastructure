// Sources of law: what kind each is, whom and when it binds, which prevails, and whether a custom exists.
//
// International law's sources are those of Article 38(1) of the Statute of the International Court of Justice: (a)
// conventions, (b) custom "as evidence of a general practice accepted as law", (c) the general principles of law, and (d)
// judicial decisions and the teachings of publicists "as subsidiary means for the determination of rules of law". Domestic
// orders rank their own (a constitution, statutes, regulations), and the ranking is a declared number here, lower being
// higher. A source is also a record: its authentic text is a holding of an archive, named by its digest.
//
// Three questions are answered.
// - Whom a treaty binds, and when: its parties, while it is in force (pacta tertiis nec nocent nec prosunt: Vienna
//   Convention on the Law of Treaties 1969, Article 34).
// - Which of two sources prevails: by rank (lex superior), by one being special to the other (lex specialis), by date (lex
//   posterior), in an order of maxims the caller states, since their order is itself argued.
// - Whether a custom exists. It has two elements, a general practice and its acceptance as law (opinio juris), each to be
//   ascertained separately (International Law Commission 2018, Draft conclusions on identification of customary
//   international law, conclusions 2, 8 and 9); a State that objected persistently while the rule was forming is not bound by
//   it (conclusion 15). The law gives no numbers for "general" or "consistent": the shares asked are a scheme the caller
//   declares, and the answer gives the measures found beside the verdict.

#pragma once

#include <apodeixis/kernel/verdict.hpp>

#include <cmath>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace law::sources {

using apodeixis::kernel::Verdict;

enum class Kind { treaty, custom, principle, decision, teaching, constitution, statute, regulation };

inline std::string_view word(Kind k) {
  switch (k) {
    case Kind::treaty: return "treaty";
    case Kind::custom: return "custom";
    case Kind::principle: return "general principle";
    case Kind::decision: return "judicial decision";
    case Kind::teaching: return "teaching";
    case Kind::constitution: return "constitution";
    case Kind::statute: return "statute";
    default: return "regulation";
  }
}

// Article 38(1)(d): decisions and teachings determine rules; they are not themselves where rules come from.
inline bool subsidiary(Kind k) { return k == Kind::decision || k == Kind::teaching; }

struct Source {
  std::string id, title;
  Kind kind = Kind::statute;
  std::string from, until;         // in force from, and until (empty: still), as dates YYYY-MM-DD
  int rank = 0;                    // within a domestic order: lower is higher
  std::set<std::string> parties;   // of a treaty
  std::string holding;             // the digest of its authentic text in an archive
};

inline bool in_force(const Source& s, const std::string& date) { return (s.from.empty() || s.from <= date) && (s.until.empty() || date <= s.until); }

// Whether a treaty binds a State on a date: the State is a party and the treaty is in force.
inline Verdict binds(const Source& treaty, const std::string& state, const std::string& date) {
  const std::string claim = treaty.title + " binds " + state + " on " + date;
  if (treaty.kind != Kind::treaty) return Verdict::no(claim, "it is no treaty");
  if (!in_force(treaty, date)) return Verdict::no(claim, "it is not in force on that date");
  if (!treaty.parties.count(state)) return Verdict::no(claim, state + " is not a party, and a treaty creates neither obligations nor rights for a third State without its consent");
  return Verdict::yes(claim, state + " is a party, and the treaty is in force");
}

// ---- Which source prevails --------------------------------------------------------------------------------------------------------

enum class Maxim { superior, specialis, posterior };

inline std::string_view word(Maxim m) { return m == Maxim::superior ? "lex superior" : m == Maxim::specialis ? "lex specialis" : "lex posterior"; }

struct Prevails {
  bool decided = false;
  std::string winner, loser;
  Maxim by = Maxim::superior;
};

// Which of two sources prevails, by the maxims in the order given. `special` holds pairs (a, b): a is special to b.
inline Prevails prevails(const Source& a, const Source& b, const std::vector<Maxim>& order = {Maxim::superior, Maxim::specialis, Maxim::posterior},
                         const std::set<std::pair<std::string, std::string>>& special = {}) {
  for (const Maxim m : order) {
    if (m == Maxim::superior && a.rank != b.rank) return {true, a.rank < b.rank ? a.id : b.id, a.rank < b.rank ? b.id : a.id, m};
    if (m == Maxim::specialis) {
      if (special.count({a.id, b.id})) return {true, a.id, b.id, m};
      if (special.count({b.id, a.id})) return {true, b.id, a.id, m};
    }
    if (m == Maxim::posterior && !a.from.empty() && !b.from.empty() && a.from != b.from) return {true, a.from > b.from ? a.id : b.id, a.from > b.from ? b.id : a.id, m};
  }
  return {};
}

// ---- Custom ----------------------------------------------------------------------------------------------------------------------

// One instance of a State's practice bearing on a putative rule.
struct Practice {
  std::string state, date;
  bool conforming = true;       // it follows the putative rule
  bool accepted_as_law = false; // it is accompanied by evidence that the State acts from a sense of legal right or obligation
  bool objection = false;       // it is an objection to the rule
};

// What "general", "consistent" and "accepted" are taken to mean: shares the caller declares.
struct Scheme {
  double generality = 0.5;   // the share of the States concerned that must have practised
  double consistency = 0.8;  // the share of the instances that must conform
  double acceptance = 0.5;   // the share of the practising States whose practice is accepted as law
};

struct Custom {
  Verdict verdict;
  double generality = 0, consistency = 0, acceptance = 0;  // the measures found
  bool practice = false, opinio_juris = false;             // each element, by the scheme
  std::set<std::string> objectors;                         // persistent objectors: the rule does not bind them
};

// Whether a custom exists among the States concerned, by a declared scheme. A State is a persistent objector when every
// instance of its practice before `formed` (the date by which the rule is taken to have formed) is an objection, and it has
// one. An objector's instances count against neither element.
inline Custom identify(const std::vector<Practice>& practice, const std::set<std::string>& concerned, const Scheme& scheme, const std::string& formed) {
  Custom c;
  std::map<std::string, std::vector<const Practice*>> by_state;
  for (const auto& p : practice)
    if (concerned.count(p.state)) by_state[p.state].push_back(&p);
  for (const auto& [state, ps] : by_state) {
    bool all = true, any = false;
    for (const auto* p : ps)
      if (p->date <= formed) any = true, all = all && p->objection;
    if (any && all) c.objectors.insert(state);
  }
  std::size_t practising = 0, accepting = 0, instances = 0, conforming = 0;
  for (const auto& [state, ps] : by_state) {
    if (c.objectors.count(state)) continue;
    bool practised = false, accepted = false;
    for (const auto* p : ps) {
      if (p->objection) continue;
      instances++;
      conforming += p->conforming;
      practised = practised || p->conforming;
      accepted = accepted || (p->conforming && p->accepted_as_law);
    }
    practising += practised;
    accepting += accepted;
  }
  const std::size_t relevant = concerned.size() - c.objectors.size();
  c.generality = relevant ? static_cast<double>(practising) / static_cast<double>(relevant) : 0;
  c.consistency = instances ? static_cast<double>(conforming) / static_cast<double>(instances) : 0;
  c.acceptance = practising ? static_cast<double>(accepting) / static_cast<double>(practising) : 0;
  c.practice = c.generality >= scheme.generality && c.consistency >= scheme.consistency;
  c.opinio_juris = c.acceptance >= scheme.acceptance;
  char buf[200];
  std::snprintf(buf, sizeof buf, "practice by %.0f%% of the States concerned, %.0f%% of instances conforming, accepted as law by %.0f%% of those practising", 100 * c.generality, 100 * c.consistency, 100 * c.acceptance);
  const std::string claim = "a custom exists among the " + apodeixis::kernel::count(concerned.size(), "State") + " concerned";
  if (c.practice && c.opinio_juris) c.verdict = Verdict::yes(claim, std::string("both elements are met under the declared scheme: ") + buf, instances);
  else c.verdict = Verdict::no(claim, std::string(!c.practice && !c.opinio_juris ? "neither element is met" : !c.practice ? "the practice is not general and consistent enough" : "the practice is general, and is not accepted as law: a usage, not a custom") + " under the declared scheme: " + buf, instances);
  for (const auto& o : c.objectors) c.verdict.with("persistent objector", o);
  return c;
}

// Whether a custom binds a State: it exists, and the State is not a persistent objector.
inline bool binds(const Custom& c, const std::string& state) { return c.verdict.holds() && !c.objectors.count(state); }

}  // namespace law::sources
