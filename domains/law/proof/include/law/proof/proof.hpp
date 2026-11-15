// Proof: burdens and standards, and whether a fact is proved.
//
// A party that asserts a fact bears the burden of proving it, to the standard the kind of case requires; a fact not proved to
// that standard is taken against the party that bore the burden (the rule on non liquet: the judge decides all the same).
// Standards are institutions, and their content is disputed: common-law courts speak of a preponderance of the evidence, of
// clear and convincing evidence, of proof beyond reasonable doubt, and decline to give them numbers; civil-law courts of the
// judge's conviction. So a standard here is declared by whoever uses it, as a test in one or both of two measures that the
// evidence foundation supplies (tekmerion): a probability that must be exceeded, and a standing the fact's argument must
// reach in the chart of evidence (justified; or at least not overruled), after the proof standards of Gordon, Prakken and
// Walton (2007, The Carneades model of argument and burden of proof, Artificial Intelligence 171), which are tests on
// arguments and not on numbers. Nothing here says what number a standard is.

#pragma once

#include <apodeixis/argumentation/aspic.hpp>
#include <apodeixis/kernel/verdict.hpp>
#include <law/facts/facts.hpp>

#include <cmath>
#include <cstdio>
#include <optional>
#include <string>

namespace law::proof {

using apodeixis::kernel::Verdict;
namespace aa = apodeixis::argumentation;

enum class ArgumentTest { none, not_overruled, justified };

struct Standard {
  std::string name;
  std::optional<double> probability;  // the probability to be exceeded, where the standard is given one
  ArgumentTest argument = ArgumentTest::none;
};

struct Burden {
  std::string fact, party, standard;  // who must prove what, to which standard (by name)
};

// What the evidence gives for a fact: its probability, and its standing as argument; either may be absent.
struct Assessment {
  std::optional<double> probability;
  std::optional<aa::Standing> standing;
};

// Whether the assessment meets the standard. Undecided when the standard asks for a measure the assessment does not have.
inline Verdict meets(const Standard& s, const Assessment& a) {
  const std::string claim = "the fact is proved to the standard \"" + s.name + "\"";
  if (s.probability && !a.probability) return Verdict::unknown(claim, "the standard asks for a probability, and none is given");
  if (s.argument != ArgumentTest::none && !a.standing) return Verdict::unknown(claim, "the standard asks how the fact's argument stands, and that is not given");
  if (s.probability && !(*a.probability > *s.probability)) {
    char buf[96];
    std::snprintf(buf, sizeof buf, "its probability, %.4g, does not exceed %.4g", *a.probability, *s.probability);
    return Verdict::no(claim, buf);
  }
  if (s.argument == ArgumentTest::justified && *a.standing != aa::Standing::justified) return Verdict::no(claim, "its argument is " + std::string(aa::word(*a.standing)) + ", and the standard asks that it be justified");
  if (s.argument == ArgumentTest::not_overruled && (*a.standing == aa::Standing::overruled || *a.standing == aa::Standing::unsupported))
    return Verdict::no(claim, "its argument is " + std::string(aa::word(*a.standing)));
  return Verdict::yes(claim, "it meets what the standard asks");
}

struct Outcome {
  facts::Standing standing = facts::Standing::not_proved;
  std::string against;  // the party the point goes against when the fact is not proved
  Verdict verdict;
};

// The fact's standing after the evidence is weighed: proved, or not proved and taken against the party that bore the burden.
// An undecided verdict (a measure missing) leaves it alleged.
inline Outcome decide(const Burden& b, const Standard& s, const Assessment& a) {
  Outcome o;
  o.verdict = meets(s, a);
  if (o.verdict.holds()) o.standing = facts::Standing::proved;
  else if (o.verdict.fails()) o.standing = facts::Standing::not_proved, o.against = b.party;
  else o.standing = facts::Standing::alleged;
  return o;
}

}  // namespace law::proof
