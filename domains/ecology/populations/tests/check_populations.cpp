// Checks of ecology/populations: the matrix of a stage model entry by entry; one stage, where λ = σ + f; vital-rate
// sensitivities against central differences of λ; the elasticities of the matrix summing to one; life expectancy 1/(1 − σ)
// for one stage and R₀ = Σ f_j l_j along a chain of stages; projection growing as λ^t towards the stable structure; the
// equilibrium under a constant input reached from any start; demographic stochasticity whose mean follows the projection
// within its Monte Carlo error; refusals.

#include <ecology/populations/stages.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace ecology::populations;

int main() {
  calx::verification::Suite s;

  const StageModel m{{"small", "medium", "large"}, {0.7, 0.8, 0.9}, {0.2, 0.1, 0.0}, {0.0, 0.5, 1.2}};
  const auto A = matrix(m);
  {
    s.check("matrix: T_00 = σ₀(1 − γ₀)", A(0, 0) - 0.7 * 0.8, 1e-15);
    s.check("matrix: T_10 = σ₀γ₀", A(1, 0) - 0.7 * 0.2, 1e-15);
    s.check("matrix: A_01 = T_01 + f₁ = f₁", A(0, 1) - 0.5, 1e-15);
    s.check("matrix: A_22 = σ₂", A(2, 2) - 0.9, 1e-15);
    s.check("matrix: A_02 = f₂", A(0, 2) - 1.2, 1e-15);
  }
  {
    const StageModel one{{"all"}, {0.6}, {0.0}, {0.55}};
    const auto a = analyse(one);
    s.check("one stage: λ = σ + f", a.lambda - 1.15, 1e-14);
    s.check("one stage: life expectancy 1/(1 − σ)", a.life_expectancy[0] - 1 / 0.4, 1e-12);
    s.check("one stage: R₀ = f/(1 − σ)", a.r0 - 0.55 / 0.4, 1e-12);
  }
  {
    const auto a = analyse(m);
    double worst = 0;
    const double h = 1e-6;
    for (std::size_t j = 0; j < 3; j++) {
      auto up = m, dn = m;
      up.survival[j] += h, dn.survival[j] -= h;
      worst = std::max(worst, std::fabs((analyse(up).lambda - analyse(dn).lambda) / (2 * h) - a.rates.survival[j]));
      up = m, dn = m;
      up.fecundity[j] += h, dn.fecundity[j] = std::max(0.0, dn.fecundity[j] - h);
      const double fd = (analyse(up).lambda - analyse(dn).lambda) / (up.fecundity[j] - dn.fecundity[j]);
      worst = std::max(worst, std::fabs(fd - a.rates.fecundity[j]));
      if (j < 2) {
        up = m, dn = m;
        up.growth[j] += h, dn.growth[j] -= h;
        worst = std::max(worst, std::fabs((analyse(up).lambda - analyse(dn).lambda) / (2 * h) - a.rates.growth[j]));
      }
    }
    s.check("vital rates: ∂λ/∂σ, ∂λ/∂γ, ∂λ/∂f against central differences", worst, 1e-6);
    double sum = 0;
    for (double e : a.elasticity.a) sum += e;
    s.check("elasticities of the matrix sum to one", sum - 1, 1e-12);
    // R₀ by survivorship along the chain: the expected offspring Σ_j f_j × (expected steps in j).
    double r0 = 0;
    const auto N = calx::linalg::fundamental(transitions(m));
    for (std::size_t j = 0; j < 3; j++) r0 += m.fecundity[j] * N(j, 0);
    s.check("R₀ = Σ_j f_j N_j0 for a population entering in the first stage (F of rank one)", a.r0 - r0, 1e-12);
    s.require("R₀ > 1 exactly when λ > 1", (a.r0 > 1) == (a.lambda > 1));
    // Projection: n(t)/n(t−1) → λ, and the structure → w.
    const auto path = project(A, {10, 0, 0}, 200);
    double t1 = 0, t0 = 0;
    for (double x : path[200]) t1 += x;
    for (double x : path[199]) t0 += x;
    s.check("projection: growth ratio → λ", t1 / t0 - a.lambda, 1e-10);
    double worst_w = 0;
    for (std::size_t j = 0; j < 3; j++) worst_w = std::max(worst_w, std::fabs(path[200][j] / t1 - a.stable[j]));
    s.check("projection: structure → the stable structure", worst_w, 1e-10);
  }
  {
    // A planted population: no fecundity, a constant planting of 20 into the first stage.
    const StageModel planted{{"young", "mature", "old"}, {0.92, 0.97, 0.95}, {0.15, 0.05, 0.0}, {0, 0, 0}};
    const auto P = matrix(planted);
    const std::vector<double> b{20, 0, 0};
    const auto star = equilibrium(P, b);
    const auto from_zero = project(P, {0, 0, 0}, 3000, {b});
    const auto from_many = project(P, {500, 900, 400}, 3000, {b});
    double worst = 0;
    for (std::size_t j = 0; j < 3; j++) worst = std::max({worst, std::fabs(from_zero[3000][j] - star[j]), std::fabs(from_many[3000][j] - star[j])});
    s.check("equilibrium: (I − A)⁻¹ b reached from any start", worst, 1e-8);
    // n* = N b: the total equals b × the expected life of a planted member.
    const auto life = analyse(planted).life_expectancy;
    s.check("equilibrium: the total is the planting times a planted member's expected life", star[0] + star[1] + star[2] - 20 * life[0], 1e-9);
    // The sustained total's sensitivities against central differences.
    const auto sus = sustained(planted, b);
    s.check("sustained: the total equals the equilibrium's", sus.total - (star[0] + star[1] + star[2]), 1e-9);
    double worst_s = 0;
    const double h = 1e-6;
    for (std::size_t j = 0; j < 3; j++) {
      auto up = planted, dn = planted;
      up.survival[j] += h, dn.survival[j] -= h;
      worst_s = std::max(worst_s, std::fabs((sustained(up, b).total - sustained(dn, b).total) / (2 * h) - sus.survival[j]) / sus.total);
      if (j < 2) {
        up = planted, dn = planted;
        up.growth[j] += h, dn.growth[j] -= h;
        worst_s = std::max(worst_s, std::fabs((sustained(up, b).total - sustained(dn, b).total) / (2 * h) - sus.growth[j]) / sus.total);
      }
    }
    s.check("sustained: d total/d survival and d total/d growth against central differences (relative to the total)", worst_s, 1e-5);
    // Stochastic runs of the planted population: the mean against the projection.
    const auto runs = simulate(planted, {0, 0, 0}, 60, 4000, 11, {20, 0, 0});
    const auto det = project(P, {0, 0, 0}, 60, {b});
    double worst_rel = 0;
    for (std::size_t j = 0; j < 3; j++) worst_rel = std::max(worst_rel, std::fabs(runs.mean[60][j] - det[60][j]) / det[60][j]);
    s.check("stochastic: the mean of 4000 runs follows the projection (relative)", worst_rel, 0.01);
    s.require("stochastic: the band holds the deterministic total", runs.total_lower[60] <= det[60][0] + det[60][1] + det[60][2] && det[60][0] + det[60][1] + det[60][2] <= runs.total_upper[60]);
    s.require("stochastic: a planted population never dies out", runs.extinct == 0);
  }
  {
    bool refused = false;
    try { (void)matrix(StageModel{{"a", "b"}, {0.5, 0.5}, {0.2, 0.3}, {0, 1}}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("refusal: growth out of the last stage", refused);
    refused = false;
    try { (void)matrix(StageModel{{"a"}, {1.2}, {0}, {0}}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("refusal: a survival above one", refused);
    refused = false;
    try { (void)equilibrium(matrix(StageModel{{"a"}, {0.9}, {0}, {0.3}}), {1}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("refusal: no equilibrium for a growing population", refused);
  }
  return s.done();
}
