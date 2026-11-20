// A surrogate standing in for a costly evaluator of allocations, with intervals whose coverage is checked (ADR 0140).
//
// An agent economy is costly to run for every allocation a search proposes. A surrogate learns the evaluator's metric from runs
// of it: each allocation read as features (each recipient's amount of a resource, scaled by the pool, and its place in the order
// of grants, scaled to [0, 1], one for a recipient never granted), a Gaussian process fitted to some runs, its split conformal
// intervals calibrated on others (heuresis's `conformal`, decision 0011), and their coverage checked on runs held out from both.
// Candidates are then screened: those whose interval could hold the best are run in the true evaluator, the rest are not, and
// the best true value is chosen. The surrogate decides nothing; what it saves is the runs it screened out, and its coverage on
// the runs it was checked on is reported beside it (the survey of evolutionary allocation, Part VI §6.2 and Part VII §6.1).
#pragma once

#include <allocation/evolution/problem.hpp>

#include <heuresis/assessment/conformal.hpp>

#include <cmath>
#include <functional>
#include <string>
#include <vector>

namespace allocation::evaluators {

namespace ae = allocation::evolution;
namespace ha = heuresis::assessment;

// Each recipient's amount over `scale`, then each recipient's place in the order of grants over the recipients.
inline std::vector<double> features_of(const ae::Allocation& a, std::size_t resource, double scale) {
  if (resource >= a.amount.size()) throw ae::Refusal("a surrogate's features read a resource the allocation does not hold");
  const std::size_t n = a.amount[resource].size();
  std::vector<double> f(2 * n, 1.0);
  for (std::size_t i = 0; i < n; i++) f[i] = a.amount[resource][i] / scale;
  for (std::size_t k = 0; k < a.granted.size(); k++)
    if (a.granted[k] < n) f[n + a.granted[k]] = static_cast<double>(k) / static_cast<double>(n);
  return f;
}

struct StandIn {
  ha::Conformal intervals;
  double held_out_coverage = 0;      // on runs used neither to fit nor to calibrate
  std::size_t fitted = 0, calibrated = 0, held_out = 0;
  std::pair<double, double> guarantee;  // the theorem's band, when runs are exchangeable
};

// A surrogate of `truth` (a metric of an allocation, to be minimised) from allocations drawn by `draw`: fitted on the first
// `fit` runs, calibrated on the next `calibrate`, checked on the last `hold`.
inline StandIn stand_in(const std::function<ae::Allocation(std::size_t)>& draw, const std::function<double(const ae::Allocation&)>& truth, std::size_t resource,
                         double scale, std::size_t fit, std::size_t calibrate, std::size_t hold, double alpha) {
  std::vector<std::vector<double>> xf, xc, xh;
  std::vector<double> yf, yc, yh;
  for (std::size_t k = 0; k < fit + calibrate + hold; k++) {
    const auto a = draw(k);
    auto f = features_of(a, resource, scale);
    const double y = truth(a);
    if (k < fit) xf.push_back(std::move(f)), yf.push_back(y);
    else if (k < fit + calibrate) xc.push_back(std::move(f)), yc.push_back(y);
    else xh.push_back(std::move(f)), yh.push_back(y);
  }
  StandIn S;
  S.intervals = ha::calibrate(ha::gaussian_process(xf, yf, true), xc, yc, alpha);
  S.fitted = fit, S.calibrated = calibrate, S.held_out = hold, S.guarantee = S.intervals.guarantee();
  std::size_t in = 0;
  for (std::size_t i = 0; i < xh.size(); i++) {
    const auto iv = S.intervals(xh[i]);
    in += yh[i] >= iv.lower && yh[i] <= iv.upper;
  }
  S.held_out_coverage = xh.empty() ? 0 : static_cast<double>(in) / static_cast<double>(xh.size());
  return S;
}

// Candidates screened by the stand-in, the rest run in the true evaluator.
inline ha::Screen screen_allocations(const StandIn& s, const std::vector<ae::Allocation>& candidates, const std::function<double(const ae::Allocation&)>& truth,
                                     std::size_t resource, double scale) {
  std::vector<std::vector<double>> x;
  for (const auto& a : candidates) x.push_back(features_of(a, resource, scale));
  return ha::screen(x, s.intervals, [&](std::size_t i) { return truth(candidates[i]); });
}

}  // namespace allocation::evaluators
