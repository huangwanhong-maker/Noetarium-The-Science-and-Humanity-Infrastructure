// Issues and the plan of proof.
//
// An issue asks whether a consequence follows from the facts under the norms, and one party bears the burden of it (proof.hpp).
// Before any evidence is weighed, counsel asks what must be proved: which facts, under which readings of the open terms, would
// make the consequence follow; which facts are needed whatever way is taken; and how the other side may answer.
//
// That is a label in apodeixis's sense. The facts the issue turns on that are not yet established are made points of the case
// themselves, each read as holding or failing (a fact that holds is had; one that fails has its negation had, so that a rule asking
// for "¬f" is met), beside the open terms with their readings. The consequence's label over all these points is the set of
// minimal partial interpretations every admissible completion of which makes it follow: each is a way to establish it, a set of
// facts to prove, facts to disprove and readings to argue for, none to spare (de Kleer's environments, 1986). The label of its not
// following is the set of ways to resist it. A fact asked in every way of establishing the consequence is necessary to it.
//
// Which facts the issue turns on is read off the norms: the atoms reached from the question backwards through the rules that give
// it, the exceptions to those rules and the counts-as links, down to atoms no rule gives. Of these, the ones the record establishes
// (as an axiom or a premise of the case: norms.hpp) are taken as they stand, and those decided against (not proved, rebutted) as
// failing. The facts in play are those someone has alleged: they are open. A fact the record does not mention is absent, and is
// left as the case leaves it (neither holding nor failing) unless the caller asks that absent facts be open too: a body of law
// offers many routes to a consequence, and a plan over every fact of every route would say little and cost much.
//
// A term whose reading the interpretive arguments have fixed (a binding holding, an argument unopposed) is not argued again: the
// plan is made over the readings they leave. The space is enumerated and never sampled: above the limit the plan is refused with the
// number of points, and the facts should be established or decided first (apodeixis's invariant: undecided is said).

#pragma once

#include <apodeixis/interpretation/interpretation.hpp>
#include <law/facts/facts.hpp>
#include <law/norms/norms.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace law::issues {

namespace ai = apodeixis::interpretation;
namespace aa = apodeixis::argumentation;
using apodeixis::kernel::Verdict;

struct Issue {
  std::string id;
  std::string question;   // the consequence asked (a literal of the norms)
  std::string burden;     // the party that bears the burden of it
  std::string standard;   // the standard it must be made out to, as proof.hpp declares it
};

struct Way {
  std::vector<std::string> prove, disprove;  // facts to make out, and facts to show do not hold
  std::vector<std::string> readings;          // readings to argue for, as [term=reading]
};

struct Plan {
  std::string issue, question, burden;
  std::vector<std::string> open;              // the facts read as open: alleged, and not yet established or decided
  std::vector<std::string> given, decided;    // relevant facts the record establishes, and those it decides against
  std::vector<std::string> absent;            // relevant facts the record does not mention
  ai::Class now = ai::Class::untenable;       // the question on the record as it stands
  std::vector<Way> establish, resist;
  std::vector<std::string> must_prove;        // facts to be made out in every way of establishing it
  std::vector<std::string> must_disprove;     // facts to be shown not to hold in every way of establishing it
  Verdict verdict;
};

inline constexpr std::size_t plan_limit = 1024;

namespace detail {

// The atom of a literal: without its negation sign.
inline std::string atom(const std::string& l) { return l.rfind("¬", 0) == 0 ? aa::negate(l) : l; }

}  // namespace detail

// The facts the question turns on: atoms reached backwards from it through rules, exceptions and counts-as links, that no rule gives.
inline std::vector<std::string> relevant(const norms::Norms& n, const std::string& question) {
  std::map<std::string, std::vector<std::vector<std::string>>> gives;  // an atom, and the antecedents of each rule giving it or its negation
  std::map<std::string, std::vector<std::string>> exceptions;          // a rule, and the facts of its exceptions
  for (const auto& r : n.rules) gives[detail::atom(r.consequence)].push_back(r.elements);
  for (const auto& e : n.exceptions) exceptions[e.rule].insert(exceptions[e.rule].end(), e.elements.begin(), e.elements.end());
  for (const auto& k : n.counts) gives[detail::atom(k.as)].push_back({k.fact});
  std::set<std::string> seen;
  std::vector<std::string> base, stack = {detail::atom(question)};
  while (!stack.empty()) {
    const std::string a = stack.back();
    stack.pop_back();
    if (!seen.insert(a).second) continue;
    const auto g = gives.find(a);
    if (g == gives.end()) {
      base.push_back(a);
      continue;
    }
    for (const auto& antecedents : g->second)
      for (const auto& x : antecedents) stack.push_back(detail::atom(x));
    for (const auto& r : n.rules)
      if (detail::atom(r.consequence) == a)
        for (const auto& x : exceptions[r.id]) stack.push_back(detail::atom(x));
  }
  std::sort(base.begin(), base.end());
  return base;
}

// The plan of proof for an issue, on a case made from the record and the norms (norms::subsume), with whatever interpretive
// arguments and holdings it carries (authority.hpp).
inline Plan plan(const ai::Case& base, const facts::Record& record, const norms::Norms& n, const Issue& issue, std::size_t limit = plan_limit, bool absent_open = false) {
  Plan out;
  out.issue = issue.id, out.question = issue.question, out.burden = issue.burden;
  const std::string claim = issue.question + " can be established";
  std::map<std::string, facts::Standing> standing;
  for (const auto& f : record.facts) standing[f.id] = f.standing;
  using S = facts::Standing;
  for (const auto& a : relevant(n, issue.question)) {
    const auto it = standing.find(a);
    if (it == standing.end()) (absent_open ? out.open : out.absent).push_back(a);
    else if (it->second == S::alleged) out.open.push_back(a);
    else if (it->second == S::not_proved || it->second == S::rebutted) out.decided.push_back(a);
    else out.given.push_back(a);
  }
  // The question on the record as it stands.
  ai::Case asked = base;
  asked.queries = {issue.question};
  {
    const auto ev = ai::evaluate(asked);
    out.now = ev.answers.at(0).classification;
  }
  // The open facts made points, each holding or failing; decided facts failing.
  ai::Case c = asked;
  // A reading that the interpretive arguments fix, or set aside, is not to be argued again in the plan: where no combination of
  // readings is excluded by name, each term keeps only the readings the arguments leave it, and the arguments have done their work.
  if (c.excluded.empty()) {
    const auto left = ai::admissible(c.points, c.arguments, c.canon_order, c.undercuts, c.excluded);
    for (std::size_t k = 0; k < c.points.size(); k++) {
      std::vector<std::string> kept;
      for (const std::size_t r : left.kept[k]) kept.push_back(c.points[k].readings[r]);
      c.points[k].readings = kept;
    }
    c.arguments.clear(), c.undercuts.clear(), c.canon_order.clear();
  }
  const std::size_t terms = c.points.size();
  for (const auto& a : out.open) {
    if (ai::point_index(c, a) >= 0) {
      out.verdict = Verdict::no(claim, "the fact " + a + " has the name of an open term; rename one of them");
      return out;
    }
    c.points.push_back({a, {"holds", "fails"}, 0});
    c.theory.rules.push_back({"open: " + a + " holds", {"[" + a + "=holds]"}, a, false, 0});
    c.theory.rules.push_back({"open: " + a + " fails", {"[" + a + "=fails]"}, aa::negate(a), false, 0});
  }
  for (const auto& a : out.decided) c.theory.axioms.push_back(aa::negate(a));
  const std::size_t space = ai::space(c.points);
  if (space > limit) {
    out.verdict = Verdict::unknown(claim, std::to_string(out.open.size()) + " open facts and " + std::to_string(terms) + " open terms make " + std::to_string(space) +
                                              " combinations, above the limit of " + std::to_string(limit) + ": establish or decide some facts first");
    return out;
  }
  const auto D = ai::derivable(c.points, ai::entailment(c, issue.question), limit);
  const auto A = ai::admissible(c.points, c.arguments, c.canon_order, c.undercuts, c.excluded).admissible;
  const auto ways = [&](bool follows) {
    std::vector<char> set(D.size());
    for (std::size_t x = 0; x < D.size(); x++) set[x] = (follows ? D[x] : !D[x]) || !A[x];
    std::vector<Way> found;
    for (const auto& p : ai::label(c.points, set)) {
      bool real = false;  // an admissible completion where it does (or does not) follow
      for (std::size_t x = 0; x < D.size() && !real; x++) real = A[x] && (follows ? D[x] : !D[x]) && ai::completes(ai::decode(c.points, x), p);
      if (!real) continue;
      Way w;
      for (std::size_t k = 0; k < c.points.size(); k++) {
        if (p[k] < 0) continue;
        if (k >= terms) (p[k] == 0 ? w.prove : w.disprove).push_back(c.points[k].name);
        else w.readings.push_back("[" + c.points[k].name + "=" + c.points[k].readings[static_cast<std::size_t>(p[k])] + "]");
      }
      found.push_back(std::move(w));
    }
    return found;
  };
  out.establish = ways(true);
  out.resist = ways(false);
  const auto in_every = [&](auto member) {
    std::vector<std::string> out_;
    if (out.establish.empty()) return out_;
    for (const auto& x : member(out.establish[0])) {
      bool all = true;
      for (const auto& w : out.establish) all = all && std::find(member(w).begin(), member(w).end(), x) != member(w).end();
      if (all) out_.push_back(x);
    }
    return out_;
  };
  out.must_prove = in_every([](const Way& w) -> const std::vector<std::string>& { return w.prove; });
  out.must_disprove = in_every([](const Way& w) -> const std::vector<std::string>& { return w.disprove; });
  const auto ways_word = [](std::size_t k) { return std::to_string(k) + (k == 1 ? " way" : " ways"); };
  const auto listed = [](const std::vector<std::string>& xs) {
    std::string s;
    for (std::size_t i = 0; i < xs.size(); i++) s += (i ? ", " : "") + xs[i];
    return s;
  };
  if (out.establish.empty()) {
    out.verdict = Verdict::no(claim, "whatever of the open facts is made out, it does not follow under the admissible readings");
  } else {
    std::string reason = ways_word(out.establish.size()) + " to establish it and " + ways_word(out.resist.size()) + " to resist it";
    if (!out.must_prove.empty()) reason += "; to be made out in every way: " + listed(out.must_prove);
    if (!out.must_disprove.empty()) reason += "; to be shown not to hold in every way: " + listed(out.must_disprove);
    out.verdict = Verdict::yes(claim, reason);
  }
  return out;
}

}  // namespace law::issues
