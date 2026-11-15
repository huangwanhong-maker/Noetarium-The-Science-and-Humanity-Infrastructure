// Checks of the evidence for a claim (evidence.hpp): studies pooled as peira pools them, graded by a declared scheme with the
// factors of the synthesis read off it and a reader's beside them, set on a link of a theory and read there; a single study pooled
// by the fixed effect, with the tests that need more studies not read; refusals.

#include <policy/evidence/evidence.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace policy;

namespace {

// Written after Guyatt et al. (2008); an example of a declared scheme, not GRADE's own text.
const char* grade_like =
    "scheme after_GRADE\n"
    "levels very_low low moderate high\n"
    "start randomised high\n"
    "start observational low\n"
    "down risk_of_bias 2\ndown inconsistency 2\ndown indirectness 2\ndown imprecision 2\ndown publication_bias 2\n"
    "up large_effect 2\nup dose_response 1\nup opposing_confounding 1\n";

const char* theory_text =
    "theory meals\n"
    "levels very_low low moderate high\n"
    "node meals intervention: free meals\n"
    "node attendance outcome: attendance\n"
    "link meals -> attendance: children come for the meal\n";

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto scheme = tekmerion::warrant::scheme(grade_like);
  const std::vector<tekmerion::warrant::Rule> rules = {{"imprecision", tekmerion::warrant::Reading::crosses, {0.0}, 0},
                                                       {"inconsistency", tekmerion::warrant::Reading::heterogeneous, {0.5, 0.75}, 0},
                                                       {"publication_bias", tekmerion::warrant::Reading::small_studies, {0.1}, 10},
                                                       {"large_effect", tekmerion::warrant::Reading::large, {0.5}, 0}};
  evidence::Body body{"free meals raise attendance", "randomised", {{"s1", 0.30, 0.010}, {"s2", 0.25, 0.015}, {"s3", 0.42, 0.020}, {"s4", 0.18, 0.012}, {"s5", 0.35, 0.018}}, true, 0.95};
  const auto a = evidence::assess(body, scheme, rules, {{"risk_of_bias", 1}});
  const auto direct = peira::synthesis::random_effects(std::vector<double>{0.30, 0.25, 0.42, 0.18, 0.35}, std::vector<double>{0.010, 0.015, 0.020, 0.012, 0.018});
  s.check("five studies are pooled as peira pools them, by random effects", std::abs(a.pooled.estimate - direct.estimate) + std::abs(a.pooled.lower - direct.lower), 1e-15);
  s.require("with five studies the prediction interval and Egger's test are made", a.prediction.has_value() && a.egger.has_value());
  s.require("the interval lies above zero (no imprecision), I² is low (no inconsistency), Egger's test is not read with five studies, the effect is below 0.5: only the reader's risk of bias moves it",
            a.pooled.lower > 0 && a.pooled.heterogeneity.i2 < 0.5 && a.graded.level == "moderate" && a.graded.verdict.reason.find("not read with 5 studies") != std::string::npos, a.graded.verdict.reason);
  auto t = change::theory(theory_text);
  evidence::attach(t, "meals", "attendance", a);
  const auto read = change::assess(t, "meals", "attendance", "moderate");
  s.require("set on the link of a theory, it is read there: attendance follows from meals at moderate", read.verdict.holds() && t.links[0].evidence.grade == "moderate");

  const auto one = evidence::assess({"a single trial", "randomised", {{"only", 0.2, 0.04}}, true, 0.95}, scheme, rules);
  s.check("a single study is its own pooled effect, 0.2 ± 1.96 × 0.2", std::abs(one.pooled.lower - (0.2 - 1.959963984540054 * 0.2)), 1e-12);
  s.require("its interval contains zero: imprecision one step, from high to moderate; no prediction interval or Egger's test with one study",
            one.graded.level == "moderate" && !one.prediction && !one.egger, one.graded.verdict.reason);

  bool r1 = false, r2 = false, r3 = false;
  try { (void)evidence::assess({"none", "randomised", {}, true, 0.95}, scheme, rules); } catch (const std::invalid_argument&) { r1 = true; }
  try { evidence::attach(t, "meals", "learning", a); } catch (const std::invalid_argument&) { r2 = true; }
  try { (void)evidence::assess({"bad", "randomised", {{"x", 0.1, 0}}, true, 0.95}, scheme, rules); } catch (const std::invalid_argument&) { r3 = true; }
  s.require("a body of no study, evidence set on a link the theory lacks, and a study of no variance are refused", r1 && r2 && r3);
  return s.done();
}
