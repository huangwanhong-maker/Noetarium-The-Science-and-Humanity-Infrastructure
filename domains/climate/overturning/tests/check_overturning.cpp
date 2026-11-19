// Checks of the climate domain's overturning in two boxes.
//
// - The reduced model's folds at y± = 2/3 ± √(η² − 3)/(3η), the extrema of p(y), and at η² = 7.5 the bistable interval
//   [0.964, 1.480] (Berglund and Gentz's reading of Cessi); the restoring rate vanishing at them.
// - The equilibria as the cubic's real roots: three inside the interval, the outer two stable, one outside; each a zero of
//   the law.
// - Ginzburg and Landau's condition λ² < 4μ³/27 (their eq. 4.8) true exactly inside the interval.
// - The slow–fast pair (4.5): z relaxes to 1 and y follows the reduced law (4.6) within O(ε₀).
// - Read by kinesis's detectors: the freshwater forcing rising slowly towards the lower branch's fold, the overturning slows,
//   a signal, its eigenvalue going to +1.

#include <climate/overturning/overturning.hpp>

#include <kinesis/transitions/transitions.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace climate::overturning;
namespace kt = kinesis::transitions;

int main() {
  calx::verification::Suite s;
  const Cessi c = cessi();

  {
    const Folds f = folds(c);
    const double sq = std::sqrt(4.5) / (3 * std::sqrt(7.5));
    s.require("the folds at y± = 2/3 ± √(η² − 3)/(3η)", std::abs(f.y_upper - (2.0 / 3 + sq)) < 1e-14 && std::abs(f.y_lower - (2.0 / 3 - sq)) < 1e-14);
    s.require("bistable for 0.964 ≤ p ≤ 1.480 at η² = 7.5", std::abs(f.p_lower - 0.964) < 5e-4 && std::abs(f.p_upper - 1.480) < 5e-4,
              std::to_string(f.p_lower) + ", " + std::to_string(f.p_upper));
    // The folds are the extrema of p(y): a grid check.
    double pmin = 1e9, pmax = -1e9;
    for (int i = 0; i <= 100000; i++) {
      const double y = 0.3 + 0.7 * i / 100000.0, p = forcing_at(c, y);
      if (y < 2.0 / 3) pmax = std::max(pmax, p);
      else pmin = std::min(pmin, p);
    }
    s.require("which are the extrema of p(y)", std::abs(pmax - f.p_upper) < 1e-9 && std::abs(pmin - f.p_lower) < 1e-9);
    s.require("and where the restoring rate vanishes", std::abs(rate(c, f.y_upper)) < 1e-12 && std::abs(rate(c, f.y_lower)) < 1e-12);
  }
  {
    const auto e = equilibria(c, 1.2);
    s.require("at p = 1.2 three equilibria, the outer two stable", e.size() == 3 && e[0].stable() && !e[1].stable() && e[2].stable());
    double worst = 0;
    for (const auto& q : e) worst = std::max(worst, std::abs(tendency(c, 1.2, q.y)));
    s.require("each a zero of the law", worst < 1e-12, std::to_string(worst));
    s.require("one equilibrium at p = 0.9 and one at p = 1.6", equilibria(c, 0.9).size() == 1 && equilibria(c, 1.6).size() == 1);
  }
  {
    const double mu = c.eta2 / 3 - 1, eta = std::sqrt(c.eta2);
    const Folds f = folds(c);
    bool agree = true;
    for (double p = 0.8; p <= 1.7; p += 0.01) {
      const double lambda = eta * (p - 2.0 / 3 - 2 * c.eta2 / 27);
      const bool well = lambda * lambda < 4 * mu * mu * mu / 27;
      const bool inside = p > f.p_lower && p < f.p_upper;
      if (std::abs(p - f.p_lower) > 1e-3 && std::abs(p - f.p_upper) > 1e-3) agree &= well == inside;
    }
    s.require("Ginzburg and Landau's double well exists exactly inside the bistable interval", agree);
  }
  {
    // The slow–fast pair against the reduced law, from y = 0.3, z = 0.9, at p = 1.0 over one diffusive time.
    double y = 0.3, z = 0.9, yr = 0.3;
    const double h = c.epsilon / 20;
    const int steps = static_cast<int>(1.0 / h);
    for (int k = 0; k < steps; k++) {
      step(c, 1.0, y, z, h);
      yr += h * tendency(c, 1.0, yr);
    }
    s.require("in the slow–fast pair z relaxes to 1", std::abs(z - 1) < 0.01, std::to_string(z));
    s.require("and y follows the reduced law within O(ε₀)", std::abs(y - yr) < 50 * c.epsilon, std::to_string(std::abs(y - yr)));
  }
  {
    // Freshwater forcing rising from 1.0 to 1.46 over 40 diffusive times, sampled every 0.05, with noise.
    const std::size_t T = 800;
    const double dt = 0.05;
    const double p0 = 1.0, p1 = 1.46;
    const double y0 = equilibria(c, p0).front().y;
    calx::random::Stream rng(20261119, 135, 31, 0);
    const auto x = kt::models::sde(T, dt, y0, [&](double y, double t) { return tendency(c, p0 + (p1 - p0) * t / ((T - 1) * dt), y); }, 0.02, rng);
    kt::Options o;
    o.trend.null_draws = 79, o.trend.alternative_draws = 40, o.trend.surrogate_draws = 39, o.trend.robustness = {kt::Null::phase};
    o.trend.grid = false, o.trend.max_windows = 60, o.trend.seed = 31, o.likelihood = false;
    const kt::Assessment a = kt::assess(kt::StateSpace{}, kt::scalar_record(kt::models::times(T, dt), x), o);
    const kt::Finding* k = nullptr;
    for (const auto& f : a.findings)
      if (f.indicator == "restoring rate (AR(1))") k = &f;
    s.require("the forcing rising towards the lower branch's fold, the overturning slows, a signal", k && k->verdict == kt::Verdict::signal, k ? k->reasons.back() : "missing");
    s.require("its dominant eigenvalue going to +1", a.kind.approach == kt::Approach::stationary, a.kind.sentence);
    s.require("and the record stayed on the lower branch", *std::max_element(x.begin(), x.end()) < folds(c).y_lower + 0.1);
  }

  return s.done();
}
