// Adopt, reject, or learn more first: the decision on an intervention against the status quo, and the study worth doing before it.
//
// An option's net present value here is normal (policy::options): mean μ, standard deviation σ. Deciding now, it is adopted when
// μ > 0, worth max(μ, 0). Knowing the effect before deciding would be worth the expected value of perfect information, the unit
// normal loss σ L(|μ|/σ) (tyche::decision::evpi_normal; Raiffa and Schlaifer 1961, ch. 5). A study of n units, each of sampling
// variance s² for the effect, moves the expected net present value by a normal amount of standard deviation
// |A v N| σ_e² / √(σ_e² + s²/n), and is worth the same loss at that (tyche::decision::evsi_normal), EVSI(n), rising in n toward EVPI.
// Studying first and then deciding is worth δ (max(μ, 0) + EVSI(n)) − c(n): the decision's worth discounted by δ for the time the
// study takes, less its cost c(n) = c₀ + c₁ n paid now. The study worth doing is the n that maximises this over the sizes
// considered, every one of them examined; it is done when it is worth more than deciding now (Claxton 1999, The irrelevance of
// inference, Journal of Health Economics 18; Claxton, Palmer et al. 2012, Informing a decision framework for when NICE should
// recommend the use of health technologies only in the context of an appropriately designed programme of evidence development,
// Health Technology Assessment 16(46), for the options beyond these three).
//
// The worth of a unit of the outcome is often the least certain number of all. μ is linear in it, so the decision changes at one
// worth, the switching value v* = C/(A N μ_e); with the worth known only within bounds, the decision is robust when μ keeps its
// sign at both, and otherwise turns on the worth, which is said, with Γ-maximin's choice (the lower bound of the net value).

#pragma once

#include <policy/options/options.hpp>

#include <tyche/decision/value.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace policy::adoption {

using apodeixis::kernel::Verdict;

struct Study {
  double unit_variance = 0;              // the sampling variance of the effect per unit studied
  double fixed_cost = 0, unit_cost = 0;  // c(n) = fixed_cost + unit_cost × n
  std::size_t most = 100000;             // the largest study considered
  double delay = 1;                      // δ: the factor on the decision's worth for waiting for the study (1: waiting costs nothing)
};

inline constexpr std::size_t study_limit = 10000000;

struct Decision {
  options::Appraised option;
  double adopt = 0, reject = 0;                                         // the worth of deciding now each way
  double evpi = 0;
  double study = std::numeric_limits<double>::quiet_NaN();              // the worth of studying first, at the best size
  double evsi = std::numeric_limits<double>::quiet_NaN();               // and that study's EVSI
  std::size_t n = 0;                                                    // its size
  std::vector<std::string> chosen;                                      // "adopt", "reject", "study first": those of greatest worth
  bool robust = true;                                                   // over the worth's bounds, when given
  std::string maximin;                                                  // Γ-maximin's choice between adopting and rejecting over them
  Verdict verdict;
};

// The worth of a study of n units, and its EVSI.
inline std::pair<double, double> study_worth(const options::Appraised& a, double effect_sd, const Study& s, std::size_t n) {
  const double f = a.factor;
  if (f == 0 || effect_sd == 0) return {s.delay * std::max(a.mean, 0.0) - (s.fixed_cost + s.unit_cost * static_cast<double>(n)), 0.0};
  const double evsi = tyche::decision::evsi_normal(a.mean, std::abs(f) * effect_sd, f * f * s.unit_variance, static_cast<double>(n));
  return {s.delay * (std::max(a.mean, 0.0) + evsi) - (s.fixed_cost + s.unit_cost * static_cast<double>(n)), evsi};
}

inline Decision decide(const options::Option& o, const nomos::appraisal::Schedule& schedule, const std::optional<Study>& study = std::nullopt,
                       const std::optional<std::pair<double, double>>& worth = std::nullopt) {
  Decision d;
  d.option = options::appraise(o, schedule);
  const auto& a = d.option;
  d.adopt = a.mean, d.reject = 0;
  d.evpi = a.sd > 0 ? tyche::decision::evpi_normal(a.mean, a.sd) : 0;
  if (study) {
    const auto& s = *study;
    if (!(s.unit_variance > 0)) throw std::invalid_argument("policy::adoption: a study's sampling variance per unit is positive");
    if (!(s.delay > 0 && s.delay <= 1)) throw std::invalid_argument("policy::adoption: the factor for waiting is in (0, 1]");
    if (!(s.fixed_cost >= 0 && s.unit_cost >= 0)) throw std::invalid_argument("policy::adoption: a study's costs are not negative");
    if (s.most < 1 || s.most > study_limit) throw std::invalid_argument("policy::adoption: the sizes considered run from 1 to at most " + std::to_string(study_limit));
    for (std::size_t n = 1; n <= s.most; n++) {
      const auto [w, e] = study_worth(a, o.se, s, n);
      if (n == 1 || w > d.study) d.study = w, d.evsi = e, d.n = n;
    }
  }
  const double best = std::max({d.adopt, d.reject, std::isnan(d.study) ? -INFINITY : d.study});
  const double tie = 1e-12 * std::max(1.0, std::abs(best));
  if (d.adopt >= best - tie) d.chosen.push_back("adopt");
  if (d.reject >= best - tie) d.chosen.push_back("reject");
  if (!std::isnan(d.study) && d.study >= best - tie) d.chosen.push_back("study first");

  std::string worth_said;
  if (worth) {
    const auto [lo, hi] = *worth;
    if (!(lo <= o.value && o.value <= hi)) throw std::invalid_argument("policy::adoption: the worth of a unit lies within its bounds");
    auto at = o;
    at.value = lo;
    const double m_lo = options::appraise(at, schedule).mean;
    at.value = hi;
    const double m_hi = options::appraise(at, schedule).mean;
    d.robust = (m_lo > 0) == (m_hi > 0) && m_lo != 0 && m_hi != 0;
    d.maximin = std::min(m_lo, m_hi) > 0 ? "adopt" : "reject";
    worth_said = d.robust ? "; the choice between adopting and rejecting is the same at every worth of a unit from " + options::number(lo) + " to " + options::number(hi)
                          : "; it turns on the worth of a unit, which switches at " + options::number(a.switching) + " within the bounds " + options::number(lo) + " to " + options::number(hi) +
                                " (Γ-maximin: " + d.maximin + ")";
  }
  std::string what;
  for (std::size_t i = 0; i < d.chosen.size(); i++) what += (i ? " or " : "") + d.chosen[i];
  std::string reason = "adopting is worth " + options::number(d.adopt) + " in expectation (standard deviation " + options::number(a.sd) + "), rejecting 0";
  if (study) reason += ", studying " + std::to_string(d.n) + " units first and then deciding " + options::number(d.study) + " (its EVSI " + options::number(d.evsi) + ")";
  reason += "; perfect information would be worth " + options::number(d.evpi) + worth_said;
  d.verdict = Verdict::yes(what + " has the greatest expected worth", reason).with("choice", what);
  if (!std::isnan(a.switching)) d.verdict.with("switching worth of a unit", options::number(a.switching));
  return d;
}

}  // namespace policy::adoption
