// Norms applied to facts: subsumption, as a case for apodeixis.
//
// A legal rule joins a consequence to elements, each an ultimate fact that must hold (Tatbestand and Rechtsfolge, in the
// German terms); an exception names a rule and the facts under which it does not apply; a term of a rule may be open, with
// several readings, and a counts-as link says that under a reading a fact of the case is the fact the rule asks for (Searle
// 1995: "X counts as Y in context C"). Applying the rules to a record of facts is then a question of derivability under an
// interpretation (apodeixis's interpretation module), and this header only states it:
//
// - a fact that is final, found, admitted, noticed or deemed is an axiom of the case; one proved or presumed is an ordinary
//   premise, open to attack; one merely alleged, not proved or rebutted is absent;
// - a rule is a defeasible rule from its elements to its consequence, named by the rule; an exception is a defeasible rule
//   from its facts to "the rule does not apply";
// - a counts-as link is a strict rule from the fact and the reading to the element;
// - where two rules' sources are ranked (sources.hpp), the rule of the prevailing source is preferred;
// - the open terms are the case's open points, and the consequences asked are its queries.
//
// The answer is apodeixis's: each consequence clear, arguable or untenable over the admissible interpretations, with its label.

#pragma once

#include <apodeixis/interpretation/interpretation.hpp>
#include <law/facts/facts.hpp>
#include <law/sources/sources.hpp>

#include <map>
#include <string>
#include <vector>

namespace law::norms {

namespace ai = apodeixis::interpretation;
namespace aa = apodeixis::argumentation;

struct Rule {
  std::string id, source;              // the source of law it comes from (a sources::Source's id), where stated
  std::vector<std::string> elements;   // the facts it requires (a leading ¬ for a fact that must not hold)
  std::string consequence;
};

struct Exception {
  std::string id, rule;                // the rule it defeats
  std::vector<std::string> elements;
};

struct OpenTerm {
  std::string name;
  std::vector<std::string> readings;
};

// Under the reading `reading` of the term `term`, the fact `fact` counts as `as`.
struct CountsAs {
  std::string id, fact, term, reading, as;
};

struct Norms {
  std::vector<Rule> rules;
  std::vector<Exception> exceptions;
  std::vector<OpenTerm> terms;
  std::vector<CountsAs> counts;
  std::vector<sources::Source> sources;
  std::vector<sources::Maxim> maxims = {sources::Maxim::superior, sources::Maxim::specialis, sources::Maxim::posterior};
  std::set<std::pair<std::string, std::string>> special;  // (a, b): source a is special to source b
};

// The case that asks whether each consequence follows from the record's facts under the norms.
inline ai::Case subsume(const facts::Record& record, const Norms& n, const std::vector<std::string>& consequences) {
  ai::Case c;
  for (const auto& f : record.facts) {
    using S = facts::Standing;
    if (f.standing == S::final_ || f.standing == S::found || f.standing == S::admitted || f.standing == S::noticed || f.standing == S::deemed) c.theory.axioms.push_back(f.id);
    else if (f.standing == S::proved || f.standing == S::presumed) c.theory.premises.push_back(f.id);
  }
  for (const auto& t : n.terms) c.points.push_back({t.name, t.readings, 0});
  for (const auto& r : n.rules) {
    aa::AspicRule ar{r.id, {}, aa::literal(r.consequence), true, 0};
    for (const auto& e : r.elements) ar.antecedents.push_back(aa::literal(e));
    c.theory.rules.push_back(ar);
  }
  for (const auto& e : n.exceptions) {
    aa::AspicRule ar{e.id, {}, aa::negate(aa::applies(e.rule)), true, 0};
    for (const auto& x : e.elements) ar.antecedents.push_back(aa::literal(x));
    c.theory.rules.push_back(ar);
  }
  for (const auto& k : n.counts) c.theory.rules.push_back({k.id, {k.fact, "[" + k.term + "=" + k.reading + "]"}, k.as, false, 0});
  // Rules of ranked sources: the rule of the prevailing source is preferred.
  std::map<std::string, const sources::Source*> by_id;
  for (const auto& s : n.sources) by_id[s.id] = &s;
  for (std::size_t i = 0; i < n.rules.size(); i++)
    for (std::size_t j = i + 1; j < n.rules.size(); j++) {
      const auto a = by_id.find(n.rules[i].source), b = by_id.find(n.rules[j].source);
      if (a == by_id.end() || b == by_id.end() || a->second == b->second) continue;
      const auto p = sources::prevails(*a->second, *b->second, n.maxims, n.special);
      if (!p.decided) continue;
      const bool first = p.winner == a->second->id;
      c.theory.rule_order.push_back({n.rules[first ? j : i].id, n.rules[first ? i : j].id});  // (weaker, stronger)
    }
  for (const auto& q : consequences) c.queries.push_back(aa::literal(q));
  return c;
}

}  // namespace law::norms
