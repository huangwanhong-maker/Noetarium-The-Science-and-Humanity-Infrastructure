// What a place's price pays for: a hedonic model of prices with the spatial dependence prices carry (ADR 0088).
//
// THE MODEL (Rosen 1974, J. Political Economy 82, 34–55). A dwelling is a bundle of characteristics (its floor, its land, its age) and
// of the amenities its place gives (a station near, a park, a protected lane), and its price is their sum at implicit prices. In the
// usual semi-logarithmic form, ln P = Xβ + ε: a coefficient on a characteristic in logarithms is the price's elasticity in it; on an
// indicator, e^β − 1 is the proportional premium.
//
// SPACE. Prices near each other share what the characteristics leave out, and may move each other (an appraiser looks at the sales
// next door). The least-squares residuals are tested by Lagrange multipliers on the weights of each sale's k nearest others,
// row-standardised (tyche's spatial regression), and a model is chosen by Anselin's rule (Anselin 2005, Exploring Spatial Data with
// GeoDa, §23; after Anselin, Bera, Florax and Yoon 1996):
// - neither LM test significant at α: least squares;
// - one significant: its model (the lag for LM_lag, the error for LM_error);
// - both: the one whose robust test is significant, or, if both robust tests are, the larger robust statistic.
// Both spatial models are fitted whatever is chosen, so that they can be compared. In the lag model, a characteristic's effect has a
// direct part and an indirect part that spills through the neighbours (LeSage and Pace 2009): the impacts.
//
// What it does not do: a hedonic coefficient is an implicit price where buyers sort among places freely and nothing left out moves
// with the amenity. A spatial model addresses dependence, not that: an amenity that comes with an unmeasured advantage is credited
// with it.

#pragma once

#include <tyche/regression/linear.hpp>
#include <tyche/spatial/autocorrelation.hpp>
#include <tyche/spatial/regression.hpp>

#include <calx/linalg/rectangular.hpp>

#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

namespace urban::demand {

enum class SpatialModel { least_squares, lag, error };

struct HedonicOptions {
  std::size_t neighbours = 8;   // k nearest sales, row-standardised
  double alpha = 0.05;          // the level of the LM tests in the rule
};

struct HedonicFit {
  tyche::regression::LinearFit ols;
  double moran = 0, moran_p = 1;               // Moran's I of the least-squares residuals
  tyche::spatial::LagrangeTests tests;
  SpatialModel chosen = SpatialModel::least_squares;
  tyche::spatial::SpatialRegression lag, error;
  tyche::spatial::Impacts impacts;             // of the lag model
  // The chosen model's coefficients and standard errors, per column of X.
  std::vector<double> beta, se;
  std::vector<double> residuals;               // the chosen model's
};

// The rule's choice from the tests.
inline SpatialModel choose(const tyche::spatial::LagrangeTests& t, double alpha) {
  const bool lag = t.p_lag < alpha, err = t.p_error < alpha;
  if (!lag && !err) return SpatialModel::least_squares;
  if (lag && !err) return SpatialModel::lag;
  if (err && !lag) return SpatialModel::error;
  const bool rl = t.p_robust_lag < alpha, re = t.p_robust_error < alpha;
  if (rl && !re) return SpatialModel::lag;
  if (re && !rl) return SpatialModel::error;
  return t.robust_lag >= t.robust_error ? SpatialModel::lag : SpatialModel::error;
}

// The hedonic model of y (logarithms of prices) on X (an intercept among its columns) at places (x, y) in metres.
inline HedonicFit hedonic(const calx::linalg::Rectangular& X, std::span<const double> lnP, std::span<const double> px, std::span<const double> py, const HedonicOptions& o = {}) {
  if (lnP.size() != X.rows || px.size() != X.rows || py.size() != X.rows) throw std::invalid_argument("demand: a price and a place for every row of X");
  HedonicFit f;
  f.ols = tyche::regression::ols(X, lnP);
  const auto W = tyche::spatial::k_nearest_weights(px, py, o.neighbours);
  const auto m = tyche::spatial::morans_i(f.ols.residuals, W);
  f.moran = m.I, f.moran_p = m.p_normal;
  f.tests = tyche::spatial::lagrange_multipliers(X, lnP, W);
  f.chosen = choose(f.tests, o.alpha);
  f.lag = tyche::spatial::spatial_lag(X, lnP, W);
  f.error = tyche::spatial::spatial_error(X, lnP, W);
  f.impacts = tyche::spatial::lag_impacts(f.lag, W);
  switch (f.chosen) {
    case SpatialModel::least_squares: f.beta = f.ols.beta, f.se = f.ols.se, f.residuals = f.ols.residuals; break;
    case SpatialModel::lag: f.beta = f.lag.beta, f.se = f.lag.se, f.residuals = f.lag.residuals; break;
    case SpatialModel::error: f.beta = f.error.beta, f.se = f.error.se, f.residuals = f.error.residuals; break;
  }
  return f;
}

}  // namespace urban::demand
