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

#include <allocation/evaluators/economy.hpp>
#include <allocation/evolution/problem.hpp>
#include <allocation/evolution/search.hpp>

#include <heuresis/assessment/conformal.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <optional>
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

// ---- The stand-in inside the search (ADR 0142) ------------------------------------------------------------------------------
//
// A costly evaluator wrapped so that the search screens as it goes. Its first `warm` candidates are run in the true evaluator, and
// what they answered for `metric` (minimised) is kept with their features. From then on, every `refit` new true runs, a Gaussian
// process is fitted to the even-numbered runs and its split conformal intervals calibrated on the odd-numbered (heuresis's
// `conformal`). A new candidate whose interval lies wholly above the `keep`-th best value run so far cannot be among the best
// `keep`, with the interval's confidence, and is not run: the evaluator abstains, saying why, and an abstention is worse than
// any value (heuresis's tiers), so the candidate is never kept. Every other candidate is run; whether its true value fell in
// the interval predicted before the run is counted, and that online coverage is reported beside the runs saved.
//
// Caveat. The guarantee of split conformal intervals needs runs exchangeable with those calibrated, and a search's candidates
// drift as it goes; the online coverage is what is claimed, measured on the candidates that were run (those the stand-in let
// through), not the theorem's band. Screening reads this one metric: with other objectives, a candidate the stand-in refuses
// might have been kept for them, so it is meant for searches whose costly metric is the one that selects. The stand-in refits
// as runs arrive, so its answers depend on the order of evaluation; the composition's search evaluates in a fixed order, and a
// run is repeated exactly.

struct ScreenSpec {
  std::string metric;          // the true evaluator's objective metric the stand-in learns (minimised)
  std::size_t resource = 0;    // the features: each recipient's amount of this resource over `scale`, and its place in the order
  double scale = 1;
  std::size_t warm = 40;       // true runs before the stand-in screens
  std::size_t refit = 20;      // new true runs between refits
  std::size_t window = 240;    // the most recent true runs the stand-in is fitted and calibrated on
  double alpha = 0.1;          // intervals of confidence 1 − α
  std::size_t keep = 8;        // a candidate is run when its interval could reach the keep-th best value run
};

struct ScreenLog {
  std::size_t runs = 0, screened = 0;       // true runs made; candidates the stand-in refused
  std::size_t checked = 0, covered = 0;     // runs whose interval was predicted first; of them, inside it
  std::size_t refits = 0;
  double coverage() const { return checked ? static_cast<double>(covered) / static_cast<double>(checked) : 0.0; }
  // The state the stand-in learns from, kept between calls.
  std::vector<std::vector<double>> x;
  std::vector<double> y;
  std::optional<ha::Conformal> intervals;
  std::size_t since = 0;
};

inline ha::Evaluator screened(std::shared_ptr<const ae::Composition> c, std::string name, ha::Kind kind, ha::Standing standing, ae::AssessAllocation truth,
                              ScreenSpec spec, std::shared_ptr<ScreenLog> log) {
  if (!truth || !log) throw ae::Refusal("a screened evaluator needs the true evaluator and a log");
  if (spec.metric.empty() || !(spec.scale > 0) || !(spec.alpha > 0 && spec.alpha < 1) || spec.keep == 0 || spec.warm < 8 || spec.refit == 0 || spec.window < 8)
    throw ae::Refusal("a stand-in needs its metric, a positive scale, 0 < α < 1, something to keep, and at least eight runs to warm on and to fit");
  auto assess = [truth, spec, log](const ae::Allocation& a, calx::random::Stream& s) {
    const auto f = features_of(a, spec.resource, spec.scale);
    std::optional<ha::Interval> iv;
    if (log->intervals) {
      iv = (*log->intervals)(f);
      std::vector<double> best = log->y;
      const std::size_t k = std::min(spec.keep, best.size());
      std::nth_element(best.begin(), best.begin() + static_cast<std::ptrdiff_t>(k - 1), best.end());
      const double threshold = best[k - 1];
      if (!iv->whole_line && iv->lower > threshold) {
        log->screened++;
        return ha::Assessment::outcome(ha::Status::abstained, "screened out by the stand-in: " + spec.metric + " in [" + std::to_string(iv->lower) + ", " +
                                                                  std::to_string(iv->upper) + "] with confidence " + std::to_string(1 - spec.alpha) +
                                                                  ", above the " + std::to_string(k) + "-th best run, " + std::to_string(threshold));
      }
    }
    ha::Assessment out = truth(a, s);
    log->runs++;
    const ha::Metric* m = out.find(spec.metric);
    if (!out.ok() || !m || !std::isfinite(m->value)) return out;   // a run that failed teaches the stand-in nothing
    if (iv) log->checked++, log->covered += (m->value >= iv->lower && m->value <= iv->upper);
    log->x.push_back(f), log->y.push_back(m->value);
    if (log->x.size() > spec.window) log->x.erase(log->x.begin()), log->y.erase(log->y.begin());
    log->since++;
    if (log->y.size() >= spec.warm && (!log->intervals || log->since >= spec.refit)) {
      std::vector<std::vector<double>> xf, xc;
      std::vector<double> yf, yc;
      for (std::size_t i = 0; i < log->x.size(); i++) (i % 2 == 0 ? xf : xc).push_back(log->x[i]), (i % 2 == 0 ? yf : yc).push_back(log->y[i]);
      try {
        log->intervals = ha::calibrate(ha::gaussian_process(xf, yf, true), xc, yc, spec.alpha);
        log->refits++;
      } catch (const std::exception&) {
        // a fit that fails (runs too alike) leaves the last stand-in in place, or none
      }
      log->since = 0;
    }
    return out;
  };
  return ae::evaluator(c, std::move(name), kind, standing, assess, false, 1, "screened 1");
}

// The economy of places behind a stand-in: its money re-spent learned and screened as the search goes.
inline ha::Evaluator screened_economy(std::shared_ptr<const ae::Composition> c, EconomySpec economy, ScreenSpec spec, std::shared_ptr<ScreenLog> log,
                                      std::string name = "an economy of places") {
  if (economy.places.size() != c->recipients()) throw ae::Refusal("the economy has a place for each recipient");
  if (spec.metric.empty()) spec.metric = "local re-spending lost";
  spec.resource = economy.resource;
  return screened(c, std::move(name), ha::Kind::agent_model, ha::Standing::computed,
                  [economy](const ae::Allocation& a, calx::random::Stream&) { return read_economy(economy, a).assessment; }, spec, std::move(log));
}

}  // namespace allocation::evaluators
