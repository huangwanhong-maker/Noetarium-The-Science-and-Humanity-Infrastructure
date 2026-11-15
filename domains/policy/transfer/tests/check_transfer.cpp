// Checks of the argument from there to here (transfer.hpp): the support factors a path needs, read from the theory; a place where
// all are present (it will work), one not known (undecided, named), one absent (it will not), the principle differing; the effect to
// expect carried over or transported through strata by peira's formula, held to its closed form; refusals.

#include <policy/transfer/transfer.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace policy;
using tekmerion::warrant::Support;

namespace {

const char* text =
    "theory meals\n"
    "levels very_low low moderate high\n"
    "node meals intervention: free meals\n"
    "node attendance outcome: attendance\n"
    "node learning outcome: learning\n"
    "link meals -> attendance: children come for the meal\n"
    "support meals -> attendance: the meal is served before the first lesson\n"
    "evidence meals -> attendance: 0.10 [0.04, 0.16] moderate\n"
    "link attendance -> learning: more days in class, more learnt\n"
    "support attendance -> learning: teachers are present to teach\n"
    "evidence attendance -> learning: 0.20 [0.10, 0.30] high\n";

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto t = change::theory(text);
  const auto path = change::assess(t, "meals", "learning", "moderate").strongest;
  const auto there = peira::synthesis::random_effects(std::vector<double>{0.30, 0.25, 0.42, 0.18, 0.35}, std::vector<double>{0.010, 0.015, 0.020, 0.012, 0.018});
  const std::string f1 = "the meal is served before the first lesson", f2 = "teachers are present to teach";

  transfer::Place all{"a district", Support::stated, {{f1, Support::stated}, {f2, Support::stated}}, std::nullopt};
  const auto h = transfer::here(t, path, there, all);
  s.require("the path needs the two support factors its links name", h.needed == std::vector<std::string>{f1, f2});
  s.require("where it worked, the principle is the same and both factors are present, it will work here", h.verdict.holds(), h.verdict.reason);
  s.require("and the pooled effect is carried over as it stands, which is said", !h.transported && h.effect == there.estimate && h.verdict.reason.find("carried over") != std::string::npos);

  transfer::Place unknown{"a town", Support::stated, {{f1, Support::stated}}, std::nullopt};
  const auto u = transfer::here(t, path, there, unknown);
  s.require("with nothing said of the teachers, it is undecided, and the factor not known is named", u.verdict.undecided() && u.unknown == std::vector<std::string>{f2} && u.verdict.reason.find(f2) != std::string::npos);

  transfer::Place absent{"a village", Support::stated, {{f1, Support::against}, {f2, Support::stated}}, std::nullopt};
  const auto a = transfer::here(t, path, there, absent);
  s.require("where the meal comes after the first lesson, it will not work there, and the absent factor is named", a.verdict.fails() && a.absent == std::vector<std::string>{f1});

  transfer::Place other{"elsewhere", Support::against, {{f1, Support::stated}, {f2, Support::stated}}, std::nullopt};
  s.require("where the causal principle differs, it will not work, whatever the factors", transfer::here(t, path, there, other).verdict.fails());

  // Strata: the effect is 0.2 among the poorer pupils and 0.05 among the others; here the poorer are 30%.
  transfer::Place strata{"a city", Support::stated, {{f1, Support::stated}, {f2, Support::stated}}, transfer::Strata{{"poorer", "others"}, {0.2, 0.05}, {0.001, 0.002}, {30, 70}}};
  const auto z = transfer::here(t, path, there, strata);
  s.check("transported through strata: 0.2 × 0.3 + 0.05 × 0.7 = 0.095", std::abs(z.effect - 0.095), 1e-15);
  s.check("with the interval 0.095 ± 1.96 √(0.3² × 0.001 + 0.7² × 0.002)", std::abs(z.upper - (0.095 + 1.959963984540054 * std::sqrt(0.09 * 0.001 + 0.49 * 0.002))), 1e-12);

  bool r1 = false;
  try { (void)transfer::here(t, path, there, {"x", Support::stated, {{"sunshine", Support::stated}}, std::nullopt}); } catch (const std::invalid_argument&) { r1 = true; }
  s.require("a place that states a factor the path does not need is refused", r1);
  return s.done();
}
