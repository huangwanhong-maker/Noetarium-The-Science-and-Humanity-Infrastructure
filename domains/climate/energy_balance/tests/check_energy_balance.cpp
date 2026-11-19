// Checks of the climate domain's energy balance.
//
// - The ramp albedo's folds at their closed forms, (A + B T_o)/(1 − α_o) and (A + B T_i)/(1 − α_i); at Q = 340 W m⁻² three
//   equilibria, the warm one at (0.7 Q − A)/B, the cold at (0.38 Q − A)/B, both stable and the middle one not; one climate
//   below the warm fold and one above the cold fold.
// - On the warm branch the restoring rate is B/C whatever Q, up to the fold: nothing slows before a non-smooth fold.
// - The smooth albedo's folds, where the restoring rate at the equilibrium vanishes.
// - Read by kinesis's detectors: the smooth balance driven slowly towards its warm fold slows, a signal; the ramp balance
//   driven to just above its fold gives no falling restoring rate, the negative case.

#include <climate/energy_balance/energy_balance.hpp>

#include <kinesis/transitions/transitions.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace climate::energy_balance;
namespace kt = kinesis::transitions;

static kt::Options quick(std::uint64_t seed) {
  kt::Options o;
  o.trend.null_draws = 79;
  o.trend.alternative_draws = 40;
  o.trend.surrogate_draws = 99;
  o.trend.robustness = {kt::Null::phase, kt::Null::iaaft, kt::Null::block};
  o.trend.grid = false;
  o.trend.max_windows = 60;
  o.trend.seed = seed;
  o.likelihood = false;
  return o;
}

static const kt::Finding* find(const kt::Assessment& a, const std::string& indicator) {
  for (const auto& f : a.findings)
    if (f.indicator == indicator) return &f;
  return nullptr;
}

int main() {
  calx::verification::Suite s;

  const Balance ramp = common(Form::ramp);
  {
    const Folds f = folds(ramp);
    s.require("the warm climate is lost below (A + B T_o)/(1 − α_o) = 290.43 W m⁻²", std::abs(f.warm_lost - 203.3 / 0.7) < 1e-9, std::to_string(f.warm_lost));
    s.require("the cold climate is lost above (A + B T_i)/(1 − α_i) = 480 W m⁻²", std::abs(f.cold_lost - (203.3 - 20.9) / 0.38) < 1e-9, std::to_string(f.cold_lost));
    const auto e = equilibria(ramp, 340);
    s.require("at Q = 340 W m⁻² there are three equilibria", e.size() == 3, std::to_string(e.size()));
    s.require("the warm one at (0.7 Q − A)/B = 16.60 °C", e.size() == 3 && std::abs(e[2].T - (0.7 * 340 - 203.3) / 2.09) < 1e-6);
    s.require("the cold one at (0.38 Q − A)/B = −35.45 °C", e.size() == 3 && std::abs(e[0].T - (0.38 * 340 - 203.3) / 2.09) < 1e-6);
    s.require("the outer two stable and the middle one not", e.size() == 3 && e[0].stable() && !e[1].stable() && e[2].stable());
    s.require("one climate, the cold, below the warm fold", equilibria(ramp, 280).size() == 1 && equilibria(ramp, 280)[0].T < -10);
    s.require("one climate, the warm, above the cold fold", equilibria(ramp, 500).size() == 1 && equilibria(ramp, 500)[0].T > 0);
    bool constant = true;
    for (double Q : {291.0, 300.0, 340.0, 400.0}) {
      const auto w = equilibria(ramp, Q).back();
      constant &= std::abs(w.rate - 2.09 / 10) < 1e-12;
    }
    s.require("on the warm branch the restoring rate is B/C = 0.209 a year whatever Q: nothing slows before the fold", constant);
  }

  const Balance smooth = common(Form::smooth);
  {
    const Folds f = folds(smooth);
    s.require("the smooth balance is bistable between its folds", f.warm_lost < 340 && 340 < f.cold_lost && equilibria(smooth, 340).size() == 3,
              std::to_string(f.warm_lost) + " to " + std::to_string(f.cold_lost));
    s.require("its restoring rate at the warm fold vanishes", std::abs(rate(smooth, f.warm_lost, f.T_warm)) < 1e-6, std::to_string(rate(smooth, f.warm_lost, f.T_warm)));
    s.require("and at the cold fold", std::abs(rate(smooth, f.cold_lost, f.T_cold)) < 1e-6);
    s.require("an equilibrium at the fold's insolation is the fold's temperature", std::abs(tendency(smooth, f.warm_lost, f.T_warm)) < 1e-9);
  }

  // Read by kinesis's detectors: annual records of 1500 years with the insolation falling towards the warm fold over the
  // last few watts, where the smooth balance slows (its rate is B/C until the equilibrium nears the albedo's transition).
  {
    const std::size_t T = 1500;
    const double dt = 1.0;
    const Folds fs = folds(smooth);
    const double q0 = fs.warm_lost + 6, q1 = fs.warm_lost + 0.5;
    calx::random::Stream rng(20261119, 135, 21, 0);
    const auto x = kt::models::sde(T, dt, equilibria(smooth, q0).back().T,
                                   [&](double temp, double t) { return tendency(smooth, q0 + (q1 - q0) * t / ((T - 1) * dt), temp); }, 0.05, rng);
    const kt::Assessment a = kt::assess(kt::StateSpace{}, kt::scalar_record(kt::models::times(T, dt), x), quick(21));
    const kt::Finding* k = find(a, "restoring rate (AR(1))");
    std::string why;
    if (k) {
      why = k->reasons.front();
      for (const auto& [n, p] : k->trend.robustness) why += "; " + n + " " + std::to_string(p);
    }
    s.require("the smooth balance driven towards its warm fold slows: the restoring rate falls, a signal", k && k->verdict == kt::Verdict::signal, why);
    s.require("and its dominant eigenvalue goes to +1", a.kind.approach == kt::Approach::stationary, a.kind.sentence);
    s.require("the record stayed on the warm branch", *std::min_element(x.begin(), x.end()) > fs.T_warm - 3, std::to_string(*std::min_element(x.begin(), x.end())));

    const Folds fr = folds(ramp);
    const double r1 = fr.warm_lost + 1.0;
    calx::random::Stream rng2(20261119, 135, 22, 0);
    const auto y = kt::models::sde(T, dt, equilibria(ramp, q0).back().T,
                                   [&](double temp, double t) { return tendency(ramp, q0 + (r1 - q0) * t / ((T - 1) * dt), temp); }, 0.05, rng2);
    const kt::Assessment b = kt::assess(kt::StateSpace{}, kt::scalar_record(kt::models::times(T, dt), y), quick(22));
    const kt::Finding* kr = find(b, "restoring rate (AR(1))");
    s.require("the ramp balance driven to just above its non-smooth fold gives no falling restoring rate", kr && kr->verdict != kt::Verdict::signal,
              kr ? kr->reasons.front() : "missing");
    s.require("and the record stayed warm (above T_o) throughout, so the fold was approached and not crossed",
              *std::min_element(y.begin(), y.end()) > 0, std::to_string(*std::min_element(y.begin(), y.end())));
  }

  return s.done();
}
