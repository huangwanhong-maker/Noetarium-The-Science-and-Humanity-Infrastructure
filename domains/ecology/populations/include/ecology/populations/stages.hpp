// Stage-classified populations: a population counted by stages (sizes, ages, conditions) and moved from one step to the
// next by a non-negative matrix built from its vital rates (Lefkovitch 1965, Biometrics 21: 1–18; Caswell 2001, Matrix
// Population Models, 2nd ed., ch. 4).
//
// The standard size-classified model (Caswell 2001, ch. 4): a member of stage j survives a step with probability σ_j and,
// having survived, grows into stage j + 1 with probability γ_j (γ = 0 in the last stage); it leaves f_j new members in the
// first stage. The projection matrix is A = T + F with
//
//   T_jj = σ_j (1 − γ_j),   T_{j+1, j} = σ_j γ_j,   F_{0 j} = f_j,
//
// columns indexing the stage one comes from, so that n(t + 1) = A n(t). Its analysis is calx's (linalg/nonnegative.hpp):
// λ = ρ(A), the stable structure w and the reproductive values v, the sensitivities ∂λ/∂a_ij = v_i w_j / vᵀw and the
// elasticities. The sensitivity to a vital rate x is Σ_ij (∂λ/∂a_ij)(∂a_ij/∂x) (Caswell 2001, ch. 9, on lower-level parameters), here
//
//   ∂λ/∂σ_j = s_jj (1 − γ_j) + s_{j+1,j} γ_j,   ∂λ/∂γ_j = σ_j (s_{j+1,j} − s_jj),   ∂λ/∂f_j = s_0j,
//
// and its elasticity (x/λ) ∂λ/∂x. The transient part T alone gives the expected time a member spends in each stage before
// it dies, the fundamental matrix N = (I − T)⁻¹ (Caswell 2001, ch. 5), and R₀ = ρ(F N) the members a member leaves over its
// life (ch. 5).
//
// A population maintained from outside (street trees planted, fish stocked, migrants arriving) is the affine system
// n(t + 1) = A n(t) + b(t). With a constant input b and ρ(A) < 1 it settles at n* = (I − A)⁻¹ b, whatever its start, at the
// geometric rate ρ(A) (the system is a contraction in a norm adapted to A).
//
// Demographic stochasticity: each member's fate in a step (stays, grows, dies) is an independent multinomial draw, and its
// offspring a Poisson draw of mean f_j; the mean of many runs is the deterministic projection, and their spread what chance
// alone does to a small population (Caswell 2001, ch. 15). Draws are calx's, reproducible by seed.
#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/nonnegative.hpp>
#include <calx/random/multinomial.hpp>
#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::populations {

struct StageModel {
  std::vector<std::string> stages;
  std::vector<double> survival;   // σ_j in [0, 1]
  std::vector<double> growth;     // γ_j in [0, 1], the last 0
  std::vector<double> fecundity;  // f_j ≥ 0
  std::size_t size() const { return survival.size(); }
};

inline void validate(const StageModel& m) {
  const std::size_t k = m.size();
  if (k == 0) throw std::invalid_argument("ecology::populations: a model has at least one stage");
  if (m.growth.size() != k || m.fecundity.size() != k) throw std::invalid_argument("ecology::populations: survival, growth and fecundity for every stage");
  for (std::size_t j = 0; j < k; j++) {
    if (!(m.survival[j] >= 0 && m.survival[j] <= 1)) throw std::invalid_argument("ecology::populations: a survival lies in [0, 1]");
    if (!(m.growth[j] >= 0 && m.growth[j] <= 1)) throw std::invalid_argument("ecology::populations: a growth lies in [0, 1]");
    if (!(m.fecundity[j] >= 0)) throw std::invalid_argument("ecology::populations: a fecundity is non-negative");
  }
  if (m.growth[k - 1] != 0) throw std::invalid_argument("ecology::populations: nothing grows out of the last stage");
}

inline calx::linalg::Matrix transitions(const StageModel& m) {
  validate(m);
  const std::size_t k = m.size();
  calx::linalg::Matrix T(k);
  for (std::size_t j = 0; j < k; j++) {
    T(j, j) = m.survival[j] * (1 - m.growth[j]);
    if (j + 1 < k) T(j + 1, j) = m.survival[j] * m.growth[j];
  }
  return T;
}

inline calx::linalg::Matrix fertilities(const StageModel& m) {
  validate(m);
  calx::linalg::Matrix F(m.size());
  for (std::size_t j = 0; j < m.size(); j++) F(0, j) = m.fecundity[j];
  return F;
}

inline calx::linalg::Matrix matrix(const StageModel& m) { return transitions(m) + fertilities(m); }

struct VitalRateSensitivity {
  std::vector<double> survival, growth, fecundity;                          // ∂λ/∂x
  std::vector<double> survival_elasticity, growth_elasticity, fecundity_elasticity;  // (x/λ) ∂λ/∂x
};

struct Analysis {
  double lambda = 0, damping = 0, r0 = 0;  // r0: NaN where the transient part never ends (ρ(T) = 1)
  std::vector<double> stable, reproductive;
  calx::linalg::Matrix sensitivity, elasticity;
  bool simple = true;
  std::vector<double> life_expectancy;  // expected steps alive from each stage (column sums of N); NaN where undefined
  VitalRateSensitivity rates;
};

inline Analysis analyse(const StageModel& m) {
  const auto A = matrix(m);
  const auto T = transitions(m), F = fertilities(m);
  const auto P = calx::linalg::perron(A);
  Analysis a;
  a.lambda = P.lambda;
  a.damping = P.damping;
  a.stable = P.right;
  a.reproductive = P.left;
  a.simple = P.simple;
  const std::size_t k = m.size();
  if (P.simple) {
    a.sensitivity = calx::linalg::sensitivities(P);
    if (P.lambda > 0) a.elasticity = calx::linalg::elasticities(A, P);
    auto& r = a.rates;
    r.survival.assign(k, 0), r.growth.assign(k, 0), r.fecundity.assign(k, 0);
    r.survival_elasticity.assign(k, 0), r.growth_elasticity.assign(k, 0), r.fecundity_elasticity.assign(k, 0);
    const auto& S = a.sensitivity;
    for (std::size_t j = 0; j < k; j++) {
      const double down = j + 1 < k ? S(j + 1, j) : 0.0;
      r.survival[j] = S(j, j) * (1 - m.growth[j]) + down * m.growth[j];
      r.growth[j] = j + 1 < k ? m.survival[j] * (down - S(j, j)) : 0.0;
      r.fecundity[j] = S(0, j);
      if (P.lambda > 0) {
        r.survival_elasticity[j] = m.survival[j] * r.survival[j] / P.lambda;
        r.growth_elasticity[j] = m.growth[j] * r.growth[j] / P.lambda;
        r.fecundity_elasticity[j] = m.fecundity[j] * r.fecundity[j] / P.lambda;
      }
    }
  }
  if (calx::linalg::spectral_radius(T) < 1) {
    a.life_expectancy = calx::linalg::expected_time_to_leave(T);
    a.r0 = calx::linalg::next_generation(T, F);
  } else {
    a.life_expectancy.assign(k, NAN);
    a.r0 = NAN;
  }
  return a;
}

// n(t) for t = 0..steps under n(t + 1) = A n(t) + b(t); `input` is one vector for every step, or one per step, or empty.
inline std::vector<std::vector<double>> project(const calx::linalg::Matrix& A, std::vector<double> n, std::size_t steps,
                                                const std::vector<std::vector<double>>& input = {}) {
  const std::size_t k = A.n;
  if (n.size() != k) throw std::invalid_argument("ecology::populations::project: one count per stage");
  if (!(input.empty() || input.size() == 1 || input.size() == steps)) throw std::invalid_argument("ecology::populations::project: one input, or one per step");
  std::vector<std::vector<double>> path{n};
  for (std::size_t t = 0; t < steps; t++) {
    std::vector<double> next(k, 0.0);
    for (std::size_t i = 0; i < k; i++)
      for (std::size_t j = 0; j < k; j++) next[i] += A(i, j) * n[j];
    if (!input.empty()) {
      const auto& b = input.size() == 1 ? input[0] : input[t];
      if (b.size() != k) throw std::invalid_argument("ecology::populations::project: an input has one entry per stage");
      for (std::size_t i = 0; i < k; i++) next[i] += b[i];
    }
    n = next;
    path.push_back(n);
  }
  return path;
}

// n* = (I − A)⁻¹ b, the population a constant input sustains; refused unless ρ(A) < 1.
inline std::vector<double> equilibrium(const calx::linalg::Matrix& A, const std::vector<double>& b) {
  if (b.size() != A.n) throw std::invalid_argument("ecology::populations::equilibrium: one input per stage");
  const auto N = calx::linalg::fundamental(A);
  std::vector<double> n(A.n, 0.0);
  for (std::size_t i = 0; i < A.n; i++)
    for (std::size_t j = 0; j < A.n; j++) n[i] += N(i, j) * b[j];
  return n;
}

// What a constant input sustains, and how its total answers each rate. With N = (I - A)^-1, n* = N b and the total 1'n*,
//
//   d(1'n*)/dx = (N'1)' (dA/dx) n*,
//
// since dN/dx = N (dA/dx) N (differentiating (I - A) N = I); N'1 holds the expected steps a member of each stage has left, so the
// sensitivity to a rate is the members it acts on times the life it adds or takes. For a population that does not keep itself
// (rho(A) < 1: a planted street forest, a stocked pond) this, not lambda, says which rates matter: lambda is then only the survival
// of its longest-lived stage.
struct Sustained {
  std::vector<double> n;
  double total = 0;
  std::vector<double> survival, growth, fecundity;                                  // d total / d x
  std::vector<double> survival_elasticity, growth_elasticity, fecundity_elasticity;  // (x / total) d total / d x
};

inline Sustained sustained(const StageModel& m, const std::vector<double>& b) {
  const auto A = matrix(m);
  const std::size_t k = m.size();
  if (b.size() != k) throw std::invalid_argument("ecology::populations::sustained: one input per stage");
  const auto N = calx::linalg::fundamental(A);
  Sustained s;
  s.n.assign(k, 0.0);
  std::vector<double> u(k, 0.0);  // N'1
  for (std::size_t i = 0; i < k; i++)
    for (std::size_t j = 0; j < k; j++) s.n[i] += N(i, j) * b[j], u[j] += N(i, j);
  for (double x : s.n) s.total += x;
  s.survival.assign(k, 0), s.growth.assign(k, 0), s.fecundity.assign(k, 0);
  s.survival_elasticity.assign(k, 0), s.growth_elasticity.assign(k, 0), s.fecundity_elasticity.assign(k, 0);
  for (std::size_t j = 0; j < k; j++) {
    const double next = j + 1 < k ? u[j + 1] : 0.0;
    s.survival[j] = s.n[j] * (u[j] * (1 - m.growth[j]) + next * m.growth[j]);
    s.growth[j] = j + 1 < k ? s.n[j] * m.survival[j] * (next - u[j]) : 0.0;
    s.fecundity[j] = s.n[j] * u[0];
    if (s.total > 0) {
      s.survival_elasticity[j] = m.survival[j] * s.survival[j] / s.total;
      s.growth_elasticity[j] = m.growth[j] * s.growth[j] / s.total;
      s.fecundity_elasticity[j] = m.fecundity[j] * s.fecundity[j] / s.total;
    }
  }
  return s;
}

// Runs of the stage model with demographic stochasticity, from integer counts, with a constant integer input per step.
struct StochasticRuns {
  std::vector<std::vector<double>> mean;   // [t][stage]
  std::vector<double> total_lower, total_median, total_upper;  // 2.5%, 50%, 97.5% of the total at each t
  double extinct = 0;                      // the fraction of runs at zero at the last step
};

inline StochasticRuns simulate(const StageModel& m, const std::vector<std::int64_t>& n0, std::size_t steps, std::size_t runs,
                               std::uint64_t seed, const std::vector<std::int64_t>& input = {}) {
  validate(m);
  const std::size_t k = m.size();
  if (n0.size() != k || (!input.empty() && input.size() != k)) throw std::invalid_argument("ecology::populations::simulate: one count per stage");
  calx::random::Stream rng(seed, 0, 0, 0);
  StochasticRuns out;
  out.mean.assign(steps + 1, std::vector<double>(k, 0.0));
  std::vector<std::vector<double>> totals(steps + 1);
  std::size_t extinct = 0;
  for (std::size_t r = 0; r < runs; r++) {
    std::vector<std::int64_t> n = n0;
    for (std::size_t t = 0; t <= steps; t++) {
      double total = 0;
      for (std::size_t j = 0; j < k; j++) out.mean[t][j] += static_cast<double>(n[j]), total += static_cast<double>(n[j]);
      totals[t].push_back(total);
      if (t == steps) break;
      std::vector<std::int64_t> next(k, 0);
      for (std::size_t j = 0; j < k; j++) {
        if (n[j] == 0) continue;
        const double stay = m.survival[j] * (1 - m.growth[j]), grow = m.survival[j] * m.growth[j];
        const std::vector<double> p{stay, grow, std::max(0.0, 1 - stay - grow)};
        const auto fate = calx::random::multinomial(rng, n[j], p);
        next[j] += fate[0];
        if (j + 1 < k) next[j + 1] += fate[1];
        if (m.fecundity[j] > 0) next[0] += rng.poisson(m.fecundity[j] * static_cast<double>(n[j]));
      }
      if (!input.empty())
        for (std::size_t j = 0; j < k; j++) next[j] += input[j];
      n = next;
    }
    bool zero = true;
    for (auto x : n) zero = zero && x == 0;
    extinct += zero ? 1 : 0;
  }
  for (auto& row : out.mean)
    for (double& x : row) x /= static_cast<double>(runs);
  for (auto& v : totals) {
    std::sort(v.begin(), v.end());
    const auto q = [&](double u) { return v[std::min(v.size() - 1, static_cast<std::size_t>(u * static_cast<double>(v.size())))]; };
    out.total_lower.push_back(q(0.025));
    out.total_median.push_back(q(0.5));
    out.total_upper.push_back(q(0.975));
  }
  out.extinct = static_cast<double>(extinct) / static_cast<double>(runs);
  return out;
}

}  // namespace ecology::populations
