// Checks of the law domain's proof: declared standards met by a probability, by an argument's standing, or by both; a missing
// measure left undecided; and a fact not proved taken against the party that bore the burden.

#include <law/proof/proof.hpp>

#include <calx/verification/check.hpp>

using namespace law::proof;

int main() {
  calx::verification::Suite s;
  // Declared for these checks; the numbers are an illustration, and no court's.
  const Standard preponderance{"a preponderance of the evidence", 0.5, ArgumentTest::none};
  const Standard beyond_doubt{"beyond reasonable doubt, as declared here", 0.95, ArgumentTest::justified};
  const Standard arguable{"a case to answer", std::nullopt, ArgumentTest::not_overruled};

  s.require("a probability of 0.6 meets a preponderance, and 0.5 does not: it must be exceeded", meets(preponderance, {0.6, std::nullopt}).holds() && meets(preponderance, {0.5, std::nullopt}).fails());
  s.require("0.97 with a justified argument meets the declared standard beyond reasonable doubt", meets(beyond_doubt, {0.97, aa::Standing::justified}).holds());
  s.require("0.97 with an argument only defensible does not: what speaks against it is unanswered", meets(beyond_doubt, {0.97, aa::Standing::defensible}).fails());
  s.require("0.90 with a justified argument does not", meets(beyond_doubt, {0.90, aa::Standing::justified}).fails());
  s.require("a standard that asks for a probability is undecided without one, and one that asks for an argument's standing without that",
            meets(beyond_doubt, {std::nullopt, aa::Standing::justified}).undecided() && meets(beyond_doubt, {0.99, std::nullopt}).undecided());
  s.require("a case to answer is met by a defensible argument and not by an overruled one, whatever the probability", meets(arguable, {std::nullopt, aa::Standing::defensible}).holds() && meets(arguable, {0.99, aa::Standing::overruled}).fails());

  const Burden burden{"unpaid", "Rivers", "a preponderance of the evidence"};
  {
    const auto o = decide(burden, preponderance, {0.7, std::nullopt});
    s.require("proved: the fact stands as proved, against no one", o.standing == law::facts::Standing::proved && o.against.empty());
    const auto n = decide(burden, preponderance, {0.4, std::nullopt});
    s.require("not proved: the point goes against the party that bore the burden", n.standing == law::facts::Standing::not_proved && n.against == "Rivers");
    const auto u = decide(burden, beyond_doubt, {0.99, std::nullopt});
    s.require("a measure missing: the fact stays alleged", u.standing == law::facts::Standing::alleged && u.verdict.undecided());
  }

  return s.done();
}
