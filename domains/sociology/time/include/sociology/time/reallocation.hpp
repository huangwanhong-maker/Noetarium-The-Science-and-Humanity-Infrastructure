// sociology::time: what moving time from one activity to another would do (ADR 0104).
//
// An outcome (health, wellbeing, a score) is regressed on the day's isometric log-ratio coordinates and any
// covariates (tyche's compositional regression). Because the day is fixed at 1440 minutes, an activity's
// "effect" is meaningful only as a reallocation: minutes moved from one activity to another, the rest held.
// The table of every such move from a reference day is the isotemporal substitution table (Dumuid et al.
// 2019), here with each entry's standard error and interval from the fit, exact for the linear model.
//
// Sources: Dumuid, D. et al. (2018), Compositional data analysis for physical activity, sedentary time and
// sleep research, Statistical Methods in Medical Research 27, 3726–3738; Dumuid, D. et al. (2019), The
// compositional isotemporal substitution model, Statistical Methods in Medical Research 28, 846–857.
//
// Accuracy: the predicted change and its interval are exact functions of the least-squares fit; they are
// associations in the sample, causal only under the assumptions a design supplies (peira's).

#pragma once

#include <sociology/time/day.hpp>

#include <tyche/compositions/regression.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <span>
#include <vector>

namespace sociology::time {

using tyche::compositions::CompositionPredictor;

// The outcome regressed on the days (rows, minutes by part) and covariates; zeros replaced as declared.
inline CompositionPredictor fit_outcome(const Rectangular& days, std::span<const double> outcome, const Rectangular& covariates = {},
                                        const DayOptions& o = {}, const tyche::compositions::PredictorOptions& p = {}) {
  require(days.rows == outcome.size(), "fit_outcome", "one outcome per day");
  return tyche::compositions::fit_predictor(close_days(days, o), outcome, covariates, p);
}

struct Substitutions {
  std::size_t parts = 0;
  double minutes = 0;                 // the minutes moved
  std::vector<double> reference;      // the day the moves start from, closed to the total
  // (i, j): `minutes` moved to part i from part j. NaN on the diagonal and where part j holds no more
  // than the minutes moved.
  Rectangular change, se, lower, upper, p;
};

// The isotemporal substitution table about a reference day (the model's centre when empty).
inline Substitutions substitutions(const CompositionPredictor& m, double minutes, std::span<const double> reference = {},
                                   const DayOptions& o = {}) {
  require(std::isfinite(minutes) && minutes > 0, "substitutions", "a positive number of minutes moved");
  Substitutions s;
  s.parts = m.parts;
  s.minutes = minutes;
  if (reference.empty()) {
    s.reference = tyche::compositions::closure(m.centre, o.total);
  } else {
    require(reference.size() == m.parts, "substitutions", "a reference day of the model's parts");
    s.reference = close_day(reference, o);
  }
  const double nan = std::numeric_limits<double>::quiet_NaN();
  for (auto* t : {&s.change, &s.se, &s.lower, &s.upper, &s.p}) {
    *t = Rectangular(m.parts, m.parts);
    for (std::size_t i = 0; i < m.parts; i++)
      for (std::size_t j = 0; j < m.parts; j++) (*t)(i, j) = nan;
  }
  for (std::size_t i = 0; i < m.parts; i++)
    for (std::size_t j = 0; j < m.parts; j++) {
      if (i == j || s.reference[j] <= minutes) continue;
      const auto r = tyche::compositions::one_for_one(m, s.reference, i, j, minutes);
      s.change(i, j) = r.change.estimate;
      s.se(i, j) = r.change.se;
      s.lower(i, j) = r.change.lower;
      s.upper(i, j) = r.change.upper;
      s.p(i, j) = r.change.p;
    }
  return s;
}

}  // namespace sociology::time
