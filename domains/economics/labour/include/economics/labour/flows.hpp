// The flows of a labour market: people moving between employment, unemployment and inactivity, the stocks those flows hold steady,
// how long a spell lasts, and which flow made a change (ADR 0104).
//
// THE CHAIN. Each period a person employed (E), unemployed (U) or outside the labour force (I) stays or moves, by the transition
// probabilities P_ij of a row-stochastic 3 × 3 matrix: what the labour force surveys measure by asking the same people in successive
// quarters (Eurostat's labour market transitions; the Current Population Survey's gross flows in the United States). Read as a Markov
// chain, the stocks s_t (a row) move as s_{t+1} = s_t P.
//
//   steady state      the stationary distribution π = π P (calx's Perron vector of Pᵀ), toward which the stocks converge: the flow
//                     steady state of Shimer (2012, Rev. Econ. Dynamics 15: 127–148); with no inactivity it is u* = s/(s + f) for a
//                     separation probability s and a finding probability f, the unemployment rate the flows alone would hold
//   rates             the unemployment rate π_U/(π_E + π_U) and the participation rate π_E + π_U of the steady state
//   spells            a stay in state i lasts 1/(1 − P_ii) periods in expectation (geometric), the period in which it ends counted
//   convergence       deviations from the steady state shrink at least as fast as |λ₂|^t, λ₂ the second eigenvalue of P in modulus; the
//                     half-life ln 2 / (−ln |λ₂|) periods says how close the stocks of a quarter are to what their flows imply
//   contributions     the change of a steady-state statistic between two periods, split among the six flows by the Shapley value of
//                     the game whose players are the flows and whose worth of a set is the change when those flows take their later
//                     values (each row's staying probability taking up the rest): exact, summing to the change whatever the order, and
//                     symmetric. The usual decompositions (Petrongolo and Pissarides 2008, Econ. J. 118: 525–541; Fujita and Ramey 2009)
//                     linearise the steady state and leave a residual; this one does not (nomos's Shapley value).
//
// Caveat. A chain of three states forgets how long a person has been unemployed: the duration dependence of finding a job, and the
// heterogeneity behind it, are outside it; the steady state is what the flows of one period would hold, not a forecast.
#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/eigenvalues.hpp>
#include <calx/linalg/nonnegative.hpp>
#include <nomos/games/claims.hpp>

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>

namespace economics::labour {

enum State : std::size_t { E = 0, U = 1, I = 2 };
using Transitions = std::array<std::array<double, 3>, 3>;  // P[i][j]: from i to j, rows summing to one

inline void validate(const Transitions& P) {
  for (std::size_t i = 0; i < 3; i++) {
    double s = 0;
    for (std::size_t j = 0; j < 3; j++) {
      if (!(P[i][j] >= 0 && P[i][j] <= 1)) throw std::invalid_argument("economics::labour: transition probabilities lie in [0, 1]");
      s += P[i][j];
    }
    if (std::fabs(s - 1) > 1e-9) throw std::invalid_argument("economics::labour: each row of transitions sums to one");
  }
}

// Probabilities from counts of people moving (c[i][j] from i to j); a row of zeros is refused.
inline Transitions from_counts(const Transitions& c) {
  Transitions P{};
  for (std::size_t i = 0; i < 3; i++) {
    double s = 0;
    for (double x : c[i]) {
      if (!(x >= 0)) throw std::invalid_argument("economics::labour::from_counts: counts are non-negative");
      s += x;
    }
    if (!(s > 0)) throw std::invalid_argument("economics::labour::from_counts: a state with no one in it");
    for (std::size_t j = 0; j < 3; j++) P[i][j] = c[i][j] / s;
  }
  return P;
}

struct Steady {
  std::array<double, 3> shares{};   // π over E, U, I
  double unemployment_rate = 0, participation_rate = 0;
  std::array<double, 3> spell{};    // expected periods a stay lasts, by state
  double second_modulus = 0, half_life = 0;  // |λ₂|; ln 2 / (−ln |λ₂|) periods (∞ when |λ₂| = 1)
};

inline Steady steady(const Transitions& P) {
  validate(P);
  calx::linalg::Matrix A(3);  // columns index where one comes from: A = Pᵀ
  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 3; j++) A(j, i) = P[i][j];
  const auto pf = calx::linalg::perron(A);
  if (!pf.simple) throw std::invalid_argument("economics::labour::steady: the chain has more than one steady state (a state no one leaves or enters)");
  Steady s;
  for (std::size_t i = 0; i < 3; i++) s.shares[i] = pf.right[i];
  s.unemployment_rate = s.shares[U] / (s.shares[E] + s.shares[U]);
  s.participation_rate = s.shares[E] + s.shares[U];
  for (std::size_t i = 0; i < 3; i++) s.spell[i] = P[i][i] < 1 ? 1 / (1 - P[i][i]) : INFINITY;
  // The second modulus: the eigenvalues other than the one at 1.
  double second = 0;
  bool skipped = false;
  for (const auto& z : calx::linalg::eigenvalues(A)) {
    if (!skipped && std::abs(z - std::complex<double>(1, 0)) < 1e-9) { skipped = true; continue; }
    second = std::max(second, std::abs(z));
  }
  s.second_modulus = second;
  s.half_life = second > 0 && second < 1 ? std::log(2.0) / -std::log(second) : (second == 0 ? 0.0 : INFINITY);
  return s;
}

// The stocks a period later: s_{t+1} = s_t P.
inline std::array<double, 3> next(const std::array<double, 3>& stocks, const Transitions& P) {
  std::array<double, 3> out{};
  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 3; j++) out[j] += stocks[i] * P[i][j];
  return out;
}

// The six flows, in the order E→U, E→I, U→E, U→I, I→E, I→U.
inline constexpr std::array<std::array<std::size_t, 2>, 6> FLOWS{{{E, U}, {E, I}, {U, E}, {U, I}, {I, E}, {I, U}}};

// The transitions with the flows in `mask` at their later values and the others at their earlier ones; staying takes up the rest.
inline Transitions mixed(const Transitions& before, const Transitions& after, std::uint32_t mask) {
  Transitions P = before;
  for (std::size_t k = 0; k < 6; k++)
    if (mask >> k & 1u) P[FLOWS[k][0]][FLOWS[k][1]] = after[FLOWS[k][0]][FLOWS[k][1]];
  for (std::size_t i = 0; i < 3; i++) {
    double moved = 0;
    for (std::size_t j = 0; j < 3; j++)
      if (j != i) moved += P[i][j];
    if (moved > 1 + 1e-12) throw std::invalid_argument("economics::labour::mixed: flows out of a state exceed one when combined");
    P[i][i] = 1 - moved;
  }
  return P;
}

struct Contributions {
  double before = 0, after = 0, change = 0;
  std::array<double, 6> by_flow{};  // the Shapley shares, summing to the change
};

// The change of a steady-state statistic (the unemployment rate by default) split among the six flows.
inline Contributions contributions(const Transitions& before, const Transitions& after,
                                   const std::function<double(const Steady&)>& statistic = [](const Steady& s) { return s.unemployment_rate; }) {
  validate(before), validate(after);
  Contributions c;
  c.before = statistic(steady(before));
  c.after = statistic(steady(after));
  c.change = c.after - c.before;
  const auto phi = nomos::games::shapley(6, [&](std::uint32_t S) { return statistic(steady(mixed(before, after, S))) - c.before; });
  for (std::size_t k = 0; k < 6; k++) c.by_flow[k] = phi[k];
  return c;
}

}  // namespace economics::labour
