// Checks of criticality (criticality.hpp).
//
// - Two components failing independently with probabilities q1 and q2, every combination a scenario: in series (the loss is 1 when
//   either fails) Birnbaum's importance of the first is 1 − q2, the reliability's derivative; in parallel (1 when both fail) it is q2;
//   RAW, RRW and Fussell–Vesely by their closed forms.
// - A loss linear in the states, L = Σ a_c (1 − x_c): criticality a_c Σ_s w_s x_{s,c} and risk a_c Σ_s w_s (1 − x_{s,c}).
// - A system that answers every "failed" at once agrees with asking one component at a time; "hardened" is asked only for what a
//   scenario disturbs.
// - The weights of a curve sum to one, and integrate a loss over return periods as tyche's curve_area does: between and beyond the
//   maps by default, and with its linear part below the first map when asked.
// - Refusals: a state of the wrong size, a share outside [0, 1], a negative weight.

#include <risk_management/criticality/criticality.hpp>

#include <tyche/descriptive/exceedance.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace rc = risk_management::criticality;

namespace {
bool throws(auto&& f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
std::vector<rc::Scenario> independent(double q1, double q2) {
  std::vector<rc::Scenario> out;
  for (int a = 0; a < 2; a++)
    for (int b = 0; b < 2; b++)
      out.push_back({"", (a ? 1 - q1 : q1) * (b ? 1 - q2 : q2), {static_cast<double>(a), static_cast<double>(b)}});
  return out;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const double q1 = 0.1, q2 = 0.3;
  {
    const rc::Loss series = [](const rc::State& x) { return x[0] * x[1] < 1 ? 1.0 : 0.0; };
    const auto A = rc::assess(2, independent(q1, q2), series);
    const double EL = 1 - (1 - q1) * (1 - q2);
    const auto& I = A.components[0];
    s.check("in series the expected loss is 1 − (1 − q1)(1 − q2)", A.expected - EL, 1e-15);
    s.check("Birnbaum's importance of the first is 1 − q2, the reliability's derivative", I.birnbaum - (1 - q2), 1e-15);
    s.check("RAW is 1 / E L", I.raw - 1 / EL, 1e-14);
    s.check("RRW is E L / q2", I.rrw - EL / q2, 1e-14);
    s.check("Fussell–Vesely is (E L − q2) / E L", I.fussell_vesely - (EL - q2) / EL, 1e-14);
    s.check("criticality is 1 − E L, risk E L − q2", std::fabs(I.criticality - (1 - EL)) + std::fabs(I.risk - (EL - q2)), 1e-15);
    const rc::Loss parallel = [](const rc::State& x) { return x[0] + x[1] == 0 ? 1.0 : 0.0; };
    const auto P = rc::assess(2, independent(q1, q2), parallel);
    s.check("in parallel Birnbaum's importance of the first is q2", P.components[0].birnbaum - q2, 1e-15);
    s.check("and of the second q1", P.components[1].birnbaum - q1, 1e-15);
  }
  {
    calx::random::Stream rng(11, 1, 2, 3);
    const std::size_t n = 6;
    std::vector<double> a(n);
    for (double& v : a) v = rng.uniform(1, 10);
    std::vector<rc::Scenario> sc;
    for (int k = 0; k < 5; k++) {
      rc::State x(n);
      for (double& v : x) v = rng.uniform() < 0.5 ? 1.0 : rng.uniform();
      sc.push_back({"", rng.uniform(0.01, 0.2), x});
    }
    const rc::Loss linear = [&](const rc::State& x) {
      double L = 0;
      for (std::size_t c = 0; c < n; c++) L += a[c] * (1 - x[c]);
      return L;
    };
    const auto A = rc::assess(n, sc, linear);
    double worst = 0;
    for (std::size_t c = 0; c < n; c++) {
      double crit = 0, risk = 0;
      for (const auto& z : sc) crit += z.weight * a[c] * z.state[c], risk += z.weight * a[c] * (1 - z.state[c]);
      worst = std::max({worst, std::fabs(A.components[c].criticality - crit), std::fabs(A.components[c].risk - risk)});
    }
    s.check("a linear loss: criticality a_c Σ w x and risk a_c Σ w (1 − x), for every component", worst, 1e-12);
    const rc::FailedAll all = [&](const rc::State& x, double at) {
      std::vector<double> F(n);
      for (std::size_t c = 0; c < n; c++) F[c] = at + a[c] * x[c];
      return F;
    };
    const auto B = rc::assess(n, sc, linear, all);
    double diff = 0;
    for (std::size_t c = 0; c < n; c++) diff = std::max(diff, std::fabs(A.components[c].failed - B.components[c].failed) + std::fabs(A.components[c].hardened - B.components[c].hardened));
    s.check("answering every 'failed' at once agrees with asking one component at a time", diff, 1e-12);
    std::size_t disturbed = 0;
    for (const auto& z : sc)
      for (double v : z.state) disturbed += v < 1;
    s.require("'hardened' is asked only for what a scenario disturbs", B.evaluations == sc.size() * 2 + disturbed);
  }
  {
    const std::vector<double> T{5, 20, 100, 500}, L{2, 7, 15, 30};
    const auto area = tyche::descriptive::curve_area(T, L, 0, true);
    for (const bool linear : {false, true}) {
      const auto w = rc::curve_weights(T, linear);
      double sum = 0, integral = 0;
      for (double x : w) sum += x;
      for (std::size_t k = 0; k < T.size(); k++) integral += w[k + 1] * L[k];
      const double want = linear ? area.total() : area.within + area.beyond;
      s.check(linear ? "a curve's weights, linear below its first map, sum to one" : "a curve's weights sum to one", sum - 1, 1e-15);
      s.check(linear ? "and integrate a loss as tyche's curve_area does, below included" : "and integrate a loss as curve_area does between and beyond its maps, nothing below the first",
              integral - want, 1e-14);
    }
  }
  {
    const rc::Loss L = [](const rc::State&) { return 0.0; };
    s.require("refusals: a state of the wrong size, a share outside [0, 1], a negative weight",
              throws([&] { rc::assess(2, {{"", 1, {1}}}, L); }) && throws([&] { rc::assess(1, {{"", 1, {1.5}}}, L); }) &&
                  throws([&] { rc::assess(1, {{"", -1, {1}}}, L); }));
  }
  return s.done();
}
