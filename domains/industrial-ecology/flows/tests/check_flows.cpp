// Checks of industrial_ecology::flows: reconciliation against its closed form at one process (by hand:
// x̂ = x̃ − V Aᵀ (A V Aᵀ)⁻¹ A x̃), an unknown found from the balances with its propagated error, a
// redundant chain, unknowns the balances cannot separate, the χ² test's mean and size under simulated
// errors, a gross error found by its normalised adjustment; transfer coefficients by hand; a substance.

#include <industrial_ecology/flows/flows.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace industrial_ecology::flows;

int main() {
  calx::verification::Suite s;

  // ---- one process, three measured flows: the closed form ----
  {
    System sys{{"mill"}, {{"ore", outside, 0}, {"metal", 0, outside}, {"slag", 0, outside}}};
    const auto R = reconcile(sys, {measured(100, 5), measured(60, 3), measured(35, 3)});
    const double lambda = 5.0 / 43;  // r = 100 − 60 − 35, W = 25 + 9 + 9
    s.check("one process: x̂ = x̃ − V Aᵀ W⁻¹ r (ore 100 − 25 λ, metal 60 + 9 λ, slag 35 + 9 λ)",
            std::fabs(R.value[0] - (100 - 25 * lambda)) + std::fabs(R.value[1] - (60 + 9 * lambda)) + std::fabs(R.value[2] - (35 + 9 * lambda)), 1e-12);
    s.check("one process: χ² = r²/W = 25/43, one degree of freedom", std::fabs(R.chi2 - 25.0 / 43) + std::fabs(static_cast<double>(R.redundancy) - 1), 1e-13);
    s.check("one process: Var(x̂_ore) = 25 − 25²/43", std::fabs(R.sd[0] * R.sd[0] - (25 - 625.0 / 43)), 1e-12);
    s.check("one process: the reconciled flows balance", R.balance, 1e-12);
  }
  // ---- an unknown found from the balance, with its error ----
  {
    System sys{{"mill"}, {{"ore", outside, 0}, {"metal", 0, outside}, {"slag", 0, outside}}};
    const auto R = reconcile(sys, {measured(100, 5), measured(60, 3), unknown()});
    s.check("no redundancy: slag = ore − metal, the measurements unchanged", std::fabs(R.value[2] - 40) + std::fabs(R.value[0] - 100) + std::fabs(R.value[1] - 60), 1e-12);
    s.check("no redundancy: σ_slag = √(25 + 9)", std::fabs(R.sd[2] - std::sqrt(34.0)), 1e-12);
    s.require("no redundancy: zero degrees of freedom, and the unknown observable", R.redundancy == 0 && R.observable[2]);
  }
  // ---- a chain: an import, an internal flow unknown, an export; a stock known exactly ----
  {
    System sys{{"use", "waste"}, {{"import", outside, 0}, {"discarded", 0, 1}, {"landfilled", 1, outside}, {"added to stock", 0, outside, true}}};
    const auto R = reconcile(sys, {measured(50, 2), unknown(), measured(40, 2), exact(5)});
    // import − discarded − 5 = 0 and discarded − landfilled = 0: import − landfilled = 5 is the one redundant balance.
    s.check("chain: import − landfilled = stock added, after reconciliation", std::fabs(R.value[0] - R.value[2] - 5), 1e-12);
    s.check("chain: the two measurements split the discrepancy equally (equal σ): 47.5 and 42.5", std::fabs(R.value[0] - 47.5) + std::fabs(R.value[2] - 42.5), 1e-12);
    s.check("chain: the internal flow is found, 42.5", std::fabs(R.value[1] - 42.5), 1e-12);
    s.check("chain: χ² = 5²/8 on one degree of freedom", std::fabs(R.chi2 - 25.0 / 8), 1e-12);
  }
  // ---- unknowns the balances cannot separate ----
  {
    System sys{{"a", "b"}, {{"in", outside, 0}, {"road", 0, 1}, {"rail", 0, 1}, {"out", 1, outside}}};
    const auto R = reconcile(sys, {measured(100, 4), unknown(), unknown(), measured(90, 3)});
    s.require("two parallel unknown flows are not observable, and say so", !R.observable[1] && !R.observable[2] && std::isnan(R.value[1]) && std::isnan(R.value[2]));
    s.check("…while the measured are still reconciled: in = out, by their variances", std::fabs(R.value[0] - R.value[3]) + std::fabs(R.value[0] - (100 - 16 * 10.0 / 25)), 1e-12);
  }
  // ---- the χ² test under simulated errors; a gross error ----
  {
    // A network of six processes; the true flows balance.
    System sys;
    sys.processes = {"p0", "p1", "p2", "p3", "p4", "p5"};
    sys.variables = {{"i0", outside, 0}, {"i1", outside, 1}, {"f01", 0, 1}, {"f02", 0, 2}, {"f13", 1, 3}, {"f23", 2, 3},
                     {"f34", 3, 4}, {"f35", 3, 5}, {"f45", 4, 5}, {"o2", 2, outside}, {"o4", 4, outside}, {"o5", 5, outside}};
    // True values: i0 = 100, i1 = 50, f01 = 60, f02 = 40; f13 = 110; f23 = 30, o2 = 10; f34 = 80, f35 = 60; f45 = 50, o4 = 30; o5 = 110.
    const std::vector<double> truth{100, 50, 60, 40, 110, 30, 80, 60, 50, 10, 30, 110};
    double worst_truth = 0;
    for (double v : sys.imbalance(truth)) worst_truth = std::max(worst_truth, std::fabs(v));
    s.check("network: the true flows balance", worst_truth, 1e-12);
    calx::random::Stream rng(11, 0, 0, 0);
    const std::size_t N = 3000;
    double mean = 0, worst_balance = 0, rejected = 0;
    std::size_t dof = 0;
    for (std::size_t r = 0; r < N; r++) {
      std::vector<Observation> obs;
      for (double v : truth) obs.push_back(measured(v + 0.05 * v * rng.normal(), 0.05 * v));
      const auto R = reconcile(sys, obs);
      mean += R.chi2 / static_cast<double>(N);
      rejected += (R.p < 0.05) / static_cast<double>(N);
      worst_balance = std::max(worst_balance, R.balance);
      dof = R.redundancy;
    }
    s.require("network: six independent balances, six degrees of freedom", dof == 6);
    s.check("network: the mean of χ² is its degrees of freedom (3000 simulated surveys)", std::fabs(mean - 6), 0.25);
    s.check("network: the test rejects 5% of balanced surveys at the 5% level", std::fabs(rejected - 0.05), 0.012);
    s.check("network: every reconciled survey balances", worst_balance, 1e-9);
    std::vector<Observation> obs;
    for (double v : truth) obs.push_back(measured(v, 0.05 * v));
    obs[4].value += 10 * obs[4].sd;  // a gross error at f13
    const auto R = reconcile(sys, obs);
    std::size_t largest = 0;
    for (std::size_t j = 0; j < truth.size(); j++)
      if (std::fabs(R.normalised[j]) > std::fabs(R.normalised[largest])) largest = j;
    s.require("network: a gross error of ten σ is rejected and found at its flow by the normalised adjustment", R.p < 1e-6 && largest == 4);
  }
  // ---- transfer coefficients ----
  {
    TransferModel m;
    m.processes = {"A", "B", "C"};
    m.shares = calx::linalg::Matrix(3);
    m.shares(0, 1) = 0.8;                     // A: 80% to B, the rest out
    m.shares(1, 2) = 0.5, m.shares(1, 0) = 0.1;  // B: 50% to C, 10% back to A, 40% out
    m.accumulation = {0, 0, 0.1};             // C: 10% to its stock, 90% out
    const auto T = transfer(m, {100, 0, 0});
    const double TA = 100 / 0.92, TB = 0.8 * TA, TC = 0.5 * TB;
    s.check("transfer: T_A = 100/0.92, T_B = 0.8 T_A, T_C = 0.5 T_B", std::fabs(T.throughput[0] - TA) + std::fabs(T.throughput[1] - TB) + std::fabs(T.throughput[2] - TC), 1e-12);
    s.check("transfer: imports = exports + accumulated", T.balance, 1e-12);
    s.check("transfer: C's stock grows by 10% of its throughput", std::fabs(T.stock_change[2] - 0.1 * TC), 1e-12);
  }
  // ---- a substance carried by goods ----
  {
    System sys{{"mill"}, {{"ore", outside, 0}, {"metal", 0, outside}, {"slag", 0, outside}}};
    const std::vector<double> x{100, 60, 40};
    const auto sub = substances(sys, x, {0.02, 0.03, 0.005});  // 2.0 in; 1.8 + 0.2 out
    s.check("substance: balances when its concentrations do (2.0 = 1.8 + 0.2)", std::fabs(sub.imbalance[0]), 1e-15);
    s.check("substance: transfer coefficients 0.9 to the metal and 0.1 to the slag", std::fabs(sub.transfer(0, 1) - 0.9) + std::fabs(sub.transfer(0, 2) - 0.1), 1e-15);
    const auto off = substances(sys, x, {0.02, 0.03, 0.01});
    s.check("substance: a concentration that does not balance shows its imbalance, −0.2", std::fabs(off.imbalance[0] + 0.2), 1e-15);
  }

  return s.done();
}
