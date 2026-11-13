// Years lived in health and in dependency: health expectancies from a life table and the prevalence of a state of health, and from a
// life course of transitions between health, dependency and death (ADR 0105).
//
// SULLIVAN'S METHOD (Sullivan 1971, HSMHA Health Reports 86: 347–354; Jagger et al. 2014, Health Expectancy Calculation by the Sullivan
// Method: A Practical Guide, EHEMU Technical Report). Let a life table give the person-years L_i lived in each age group and l_x the
// survivors at x, and let π_i be the prevalence of a state (a limitation of usual activities, say) among those alive in group i.
// Health expectancy at x is the expectation of life lived outside the state,
//
//   HLE_x = (1/l_x) Σ_{i ≥ x} (1 − π_i) L_i,     DLE_x = e_x − HLE_x,
//
// and, with prevalences estimated from samples of n_i, the sampling variance Var(HLE_x) = Σ_{i ≥ x} (L_i/l_x)² π_i(1 − π_i)/n_i (Jagger
// et al. 2014, eq. for the variance under prevalence sampling alone; the life table's own sampling error is neglected, as there). A
// prevalence given in age bands coarser than the table is carried to each group by the band that holds its starting age.
//
// A LIFE COURSE OF TRANSITIONS (the increment–decrement life table; Rogers, Rogers and Branch 1989; Caswell and van Daalen 2021, Demogr.
// Res. 45: 397–452, Markov chains with rewards). Age is counted in steps of n years; at each step a person healthy (h) or dependent (d)
// moves by probabilities p_hh, p_hd, p_dh, p_dd and dies with the rest. The transient part U of the chain on (age, state), columns
// indexing where one comes from, has the fundamental matrix N = (I − U)⁻¹ (calx's), whose entry N[(y, s), (x, r)] is the expected
// number of step-starts spent at age y in state s by a person at age x in state r. With deaths spread evenly in a step (the life table's
// a = n/2), the expected years in state s from (x, r) are
//
//   Y_s(x, r) = n (Σ_y N[(y, s), (x, r)] − ½ [s = r]),
//
// the trapezoidal rule over the steps (proof: the theory note §3), so that a chain with one state gives e_x = n(Σ l_y/l_x − ½), the
// life table's with a = n/2. The last age is open: a person stays in it, moving between states, until death.
//
// SULLIVAN AND THE CHAIN AGREE in a stationary population: if the prevalence of each age group is the share of the chain's person-years
// in it spent dependent (trapezoidal), Sullivan's HLE from the chain's survivors equals the chain's Y_h. The check holds this exactly.
// Where prevalence is not stationary (incidence changing, a cohort ageing) the two part, and the chain is the one that describes a
// life course; Sullivan describes the current population's health.
#pragma once

#include <demography/lifetable/lifetable.hpp>

#include <calx/linalg/dense.hpp>
#include <calx/linalg/nonnegative.hpp>

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace demography::health {

using lifetable::LifeTable;

struct Sullivan {
  std::vector<double> x, le, hle, dle, share, se;  // by starting age: life expectancy, healthy and dependent years, HLE/LE, se(HLE)
};

// The prevalence of each group of the table from bands [band_start[k], band_start[k + 1]): each group takes its starting age's band.
inline std::vector<double> prevalence_at(const LifeTable& t, const std::vector<double>& band_start, const std::vector<double>& band_prevalence) {
  if (band_start.empty() || band_start.size() != band_prevalence.size()) throw std::invalid_argument("demography::health::prevalence_at: one prevalence a band");
  std::vector<double> p(t.x.size(), 0.0);
  for (std::size_t i = 0; i < t.x.size(); i++) {
    std::size_t k = 0;
    while (k + 1 < band_start.size() && t.x[i] >= band_start[k + 1]) k++;
    p[i] = t.x[i] < band_start[0] ? 0.0 : band_prevalence[k];  // before the first band, no one is counted in the state
  }
  return p;
}

inline Sullivan sullivan(const LifeTable& t, const std::vector<double>& prevalence, const std::vector<double>& sample_sizes = {}) {
  const std::size_t k = t.x.size();
  if (prevalence.size() != k) throw std::invalid_argument("demography::health::sullivan: one prevalence a group");
  if (!sample_sizes.empty() && sample_sizes.size() != k) throw std::invalid_argument("demography::health::sullivan: one sample size a group, or none");
  for (double p : prevalence)
    if (!(p >= 0 && p <= 1)) throw std::invalid_argument("demography::health::sullivan: a prevalence lies in [0, 1]");
  Sullivan s;
  s.x = t.x;
  s.le.assign(k, 0), s.hle.assign(k, 0), s.dle.assign(k, 0), s.share.assign(k, 0), s.se.assign(k, NAN);
  double healthy = 0, var = 0;
  for (std::size_t i = k; i-- > 0;) {
    healthy += (1 - prevalence[i]) * t.L[i];
    if (!sample_sizes.empty() && sample_sizes[i] > 0) var += t.L[i] * t.L[i] * prevalence[i] * (1 - prevalence[i]) / sample_sizes[i];
    if (t.l[i] <= 0) continue;
    s.le[i] = t.e[i];
    s.hle[i] = healthy / t.l[i];
    s.dle[i] = s.le[i] - s.hle[i];
    s.share[i] = s.le[i] > 0 ? s.hle[i] / s.le[i] : 0;
    if (!sample_sizes.empty()) s.se[i] = std::sqrt(var) / t.l[i];
  }
  return s;
}

// One age step's transitions for the healthy and the dependent; deaths are what is left.
struct Step {
  double hh = 0, hd = 0, dh = 0, dd = 0;
};

struct LifeCourse {
  double step = 1;                     // n, years
  std::vector<double> healthy, dependent;  // expected years from (age x, healthy) in each state, x = 0, n, 2n, …
  std::vector<double> healthy_from_dependent, dependent_from_dependent;  // the same from (age x, dependent)
  std::vector<double> alive_healthy, alive_dependent;  // occupancy at each age of a cohort starting healthy at age 0 (shares of it)
};

inline LifeCourse life_course(const std::vector<Step>& steps, double n = 1) {
  const std::size_t K = steps.size();
  if (K == 0 || !(n > 0)) throw std::invalid_argument("demography::health::life_course: at least one age step, of positive width");
  for (const auto& s : steps) {
    if (!(s.hh >= 0 && s.hd >= 0 && s.dh >= 0 && s.dd >= 0) || s.hh + s.hd > 1 + 1e-12 || s.dh + s.dd > 1 + 1e-12)
      throw std::invalid_argument("demography::health::life_course: transition probabilities, each of a state's summing to at most one");
  }
  // States (age y, s) at index 2y + s, s = 0 healthy, 1 dependent; the last age keeps its people.
  const std::size_t M = 2 * K;
  calx::linalg::Matrix U(M);
  for (std::size_t y = 0; y < K; y++) {
    const std::size_t to = y + 1 < K ? y + 1 : y;
    U(2 * to + 0, 2 * y + 0) += steps[y].hh;
    U(2 * to + 1, 2 * y + 0) += steps[y].hd;
    U(2 * to + 0, 2 * y + 1) += steps[y].dh;
    U(2 * to + 1, 2 * y + 1) += steps[y].dd;
  }
  const auto N = calx::linalg::fundamental(U);
  LifeCourse lc;
  lc.step = n;
  lc.healthy.assign(K, 0), lc.dependent.assign(K, 0), lc.healthy_from_dependent.assign(K, 0), lc.dependent_from_dependent.assign(K, 0);
  for (std::size_t x = 0; x < K; x++) {
    double sh = 0, sd = 0, dh = 0, dd = 0;
    for (std::size_t y = 0; y < K; y++) {
      sh += N(2 * y, 2 * x), sd += N(2 * y + 1, 2 * x);
      dh += N(2 * y, 2 * x + 1), dd += N(2 * y + 1, 2 * x + 1);
    }
    lc.healthy[x] = n * (sh - 0.5), lc.dependent[x] = n * sd;
    lc.healthy_from_dependent[x] = n * dh, lc.dependent_from_dependent[x] = n * (dd - 0.5);
  }
  // A cohort starting healthy at age 0, step by step (the open last age folds its stayers into itself).
  lc.alive_healthy.assign(K + 1, 0), lc.alive_dependent.assign(K + 1, 0);
  double h = 1, d = 0;
  for (std::size_t y = 0; y <= K; y++) {
    lc.alive_healthy[y] = h, lc.alive_dependent[y] = d;
    if (y == K) break;
    const double nh = h * steps[y].hh + d * steps[y].dh, nd = h * steps[y].hd + d * steps[y].dd;
    h = nh, d = nd;
  }
  return lc;
}

}  // namespace demography::health
