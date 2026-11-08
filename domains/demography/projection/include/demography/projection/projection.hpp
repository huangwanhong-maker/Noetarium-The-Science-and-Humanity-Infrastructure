// Populations projected by age, and the stable population they tend to (Leslie 1945; Lotka 1939; Keyfitz 1964; Keyfitz and Caswell
// 2005, Applied Mathematical Demography, chs 3–6; Preston, Heuveline and Guillot 2001, chs 6–7). The domain's theory note, §4.
//
// THE LESLIE MATRIX. Women in k age groups of width n (the groups of a life table, closed), projected n years at a time: N(t + n) =
// M N(t), with the survival ratios s_i = L_{i+1} / L_i below the diagonal and the first row
//   F_i = s (L_0 / (2 l_0)) (f_i + s_i f_{i+1}),
// the daughters, surviving to the first group, born to a woman of group i over the n years: her births at rate f_i while in the group
// and f_{i+1} after moving up, half each on average; L_0 / l_0, the person-years of the first group per birth, carries both the n
// years and the babies' survival (Keyfitz 1964; Preston et al. 2001, eq. 6.6). Then R₀ = Σ F_i Π_{j<i} s_j telescopes to the life
// table's NRR = s Σ f_x L_x / l_0 exactly when the schedule is zero at the matrix's first and last groups (checked).
//
// THE STABLE POPULATION. M is non-negative; when its fertile groups make it primitive, Perron and Frobenius give it a simple dominant
// eigenvalue λ > 0 with a positive eigenvector, and any population tends to that eigenvector's age structure while growing by λ each
// step: the strong ergodic theorem (Lopez 1961; Keyfitz and Caswell 2005, §4.5). The intrinsic rate is r = ln λ / n. λ solves the
// discrete characteristic equation Σ_i F_i (Π_{j<i} s_j) λ^{−(i+1)} = 1, and λ > 1 exactly when R₀ = Σ_i F_i Π_{j<i} s_j > 1 (checked).
// Lotka's continuous r solves Σ_x e^{−r (x + n/2)} s f_x L_x / l_0 = 1, another discretisation of the renewal equation; the two agree to
// the order of n² (compared).

#pragma once

#include <demography/lifetable/lifetable.hpp>

#include <calx/solvers/roots.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace demography::projection {

struct Leslie {
  double n = 5;
  std::vector<double> first;     // F_i, k entries
  std::vector<double> survival;  // s_i, k − 1 entries
  std::size_t size() const { return first.size(); }
};

// The Leslie matrix of the women's table's first k groups (all of width n), with rates f for those groups (zero beyond the schedule),
// and the share s of births that are girls.
inline Leslie leslie(const lifetable::LifeTable& women, std::size_t k, const std::vector<double>& f, double girls) {
  lifetable::detail::require(k >= 2 && k < women.size() && f.size() == k, "k groups, closed, and a rate for each");
  const double n = women.n[0];
  for (std::size_t i = 0; i < k; i++) lifetable::detail::require(women.n[i] == n, "groups of equal width");
  Leslie M;
  M.n = n;
  M.first.resize(k), M.survival.resize(k - 1);
  for (std::size_t i = 0; i + 1 < k; i++) M.survival[i] = women.L[i + 1] / women.L[i];
  const double baby = women.L[0] / (2 * women.l[0]);
  for (std::size_t i = 0; i < k; i++) {
    const double next = i + 1 < k ? M.survival[i] * f[i + 1] : 0.0;
    M.first[i] = girls * baby * (f[i] + next);
  }
  return M;
}

inline std::vector<double> project(const Leslie& M, const std::vector<double>& N) {
  lifetable::detail::require(N.size() == M.size(), "a population of the matrix's groups");
  std::vector<double> out(N.size(), 0.0);
  for (std::size_t i = 0; i < N.size(); i++) out[0] += M.first[i] * N[i];
  for (std::size_t i = 0; i + 1 < N.size(); i++) out[i + 1] = M.survival[i] * N[i];
  return out;
}

struct Stable {
  double lambda = 0, r = 0;            // the dominant eigenvalue and the intrinsic rate per year
  std::vector<double> structure;       // the stable age structure, summing to one
  double r0 = 0;                       // Σ F_i Π s_j
  int iterations = 0;
};

// λ and the stable structure by the power method (from a uniform population, to a relative change below tolerance).
inline Stable stable(const Leslie& M, double tolerance = 1e-14, int max_iterations = 100000) {
  const std::size_t k = M.size();
  Stable s;
  std::vector<double> N(k, 1.0 / static_cast<double>(k));
  double previous = 0;
  for (s.iterations = 0; s.iterations < max_iterations; s.iterations++) {
    auto next = project(M, N);
    double total = 0;
    for (double v : next) total += v;
    lifetable::detail::require(total > 0, "a matrix that keeps a population alive");
    for (auto& v : next) v /= total;
    s.lambda = total;  // N sums to one, so the total after one step is the growth factor
    double change = 0;
    for (std::size_t i = 0; i < k; i++) change = std::max(change, std::fabs(next[i] - N[i]));
    N = next;
    if (change < tolerance && std::fabs(s.lambda - previous) < tolerance * s.lambda) break;
    previous = s.lambda;
  }
  s.structure = N;
  s.r = std::log(s.lambda) / M.n;
  double survive = 1;
  for (std::size_t i = 0; i < k; i++) {
    s.r0 += M.first[i] * survive;
    if (i + 1 < k) survive *= M.survival[i];
  }
  return s;
}

// The discrete characteristic function Σ_i F_i (Π_{j<i} s_j) λ^{−(i+1)}, equal to one at the dominant eigenvalue.
inline double characteristic(const Leslie& M, double lambda) {
  double sum = 0, survive = 1;
  for (std::size_t i = 0; i < M.size(); i++) {
    sum += M.first[i] * survive * std::pow(lambda, -static_cast<double>(i + 1));
    if (i + 1 < M.size()) survive *= M.survival[i];
  }
  return sum;
}

// Lotka's intrinsic rate from the women's table and rates f for groups starting at x (width n): the root of
// Σ e^{−r (x + n/2)} s f L / l_0 = 1, by Brent's method on [−0.2, 0.2] per year.
inline double lotka_r(const lifetable::LifeTable& women, const std::vector<double>& x, double n, const std::vector<double>& f, double girls) {
  std::vector<double> L;
  for (const double age : x) {
    std::size_t j = 0;
    while (j < women.size() && women.x[j] != age) j++;
    lifetable::detail::require(j + 1 < women.size() && women.n[j] == n, "the women's table has each group, of the same width");
    L.push_back(women.L[j] / women.l[0]);
  }
  const auto g = [&](double r) {
    double sum = 0;
    for (std::size_t i = 0; i < x.size(); i++) sum += std::exp(-r * (x[i] + n / 2)) * girls * f[i] * L[i];
    return sum - 1;
  };
  return calx::solvers::brent(g, -0.2, 0.2, 1e-15).x;
}

}  // namespace demography::projection
