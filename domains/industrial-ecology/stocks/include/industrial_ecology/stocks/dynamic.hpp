// Dynamic stock models: what is in use, what enters it and what leaves it, cohort by cohort (theory
// note §2).
//
// Periods t = 0 .. T − 1. A cohort enters in period c with inflow I(c) and survives by its lifetime law
// S_c, holding s(t, c) = I(c) S_c(t − c) at the end of period t ≥ c. The stock, outflow and net addition:
//
//   S(t) = Σ_{c ≤ t} s(t, c) + S₀(t),       O(t) = Σ_{c ≤ t} o(t, c) + O₀(t),       S(t) − S(t − 1) = I(t) − O(t),
//
// with o(c, c) = I(c) − s(c, c) and o(t, c) = s(t − 1, c) − s(t, c) for t > c (Müller 2006, Ecol. Econ.
// 59: 142–156, eqs. 1–4). S₀ is what remains of an initial stock given by age at the end of period −1,
// each age a surviving by the conditional law S(t + 1 + a)/S(a) (Pauliuk and Heeren 2020, ODYM's
// stock-driven model with an initial stock). Mass is conserved by construction, and `balance` reports
// the largest departure, which is rounding.
//
//   inflow_driven(I, laws, initial)        the stock and the outflow that the inflows leave
//   stock_driven(S, laws, initial)         the inflow that keeps a stock on a given path: a triangular
//                                          system solved period by period, I(t) = (S(t) − what remains of
//                                          earlier cohorts at t) / S_t(0) (Müller 2006, eq. 6)
//
// A stock that falls faster than its cohorts leave asks for a negative inflow. Unless told to keep it,
// stock_driven sets that inflow to zero and retires the excess early, from every cohort in proportion
// to what it holds (each then surviving by its law, scaled); what was retired early is reported per
// period, and the stock follows its path exactly. This is ODYM's correction of negative inflows in
// substance, made explicit.
//
// Laws may differ by cohort (a vector of one law per period) or be shared (a vector of one law).

#pragma once

#include <industrial_ecology/stocks/lifetime.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace industrial_ecology::stocks {

// An initial stock at the end of period −1, by age: by_age[a] entered in period −1 − a.
struct Initial {
  std::vector<double> by_age;
  Lifetime law;  // the law its cohorts follow
};

struct Model {
  std::vector<double> inflow, stock, outflow, net_addition;
  std::vector<std::vector<double>> by_cohort;          // by_cohort[t][c], c ≤ t: what of cohort c is in use at the end of t
  std::vector<std::vector<double>> outflow_by_cohort;  // outflow_by_cohort[t][c], c ≤ t
  std::vector<std::vector<double>> initial_by_age;     // initial_by_age[t][a]: what of the initial stock's age a remains at t
  std::vector<double> initial_stock, initial_outflow;  // S₀(t), O₀(t)
  std::vector<double> retired_early;                   // stock-driven: retired early because the inflow would have been negative
  std::vector<double> mean_age;                        // the mean age of the stock at the end of each period
  std::vector<double> initial_start;                   // the initial stock by age at the end of period −1
  double initial_total = 0;                            // S(−1)
  double balance = 0;                                  // max |S(t) − S(t − 1) − (I(t) − O(t))|
};

namespace detail {

inline const Lifetime& law_of(const std::vector<Lifetime>& laws, std::size_t c) {
  require(!laws.empty(), "at least one lifetime law");
  require(laws.size() == 1 || c < laws.size(), "one lifetime law, or one per period");
  return laws.size() == 1 ? laws[0] : laws[c];
}

// The survival table of every cohort over the horizon, and the initial stock's conditional survival.
struct Tables {
  std::vector<std::vector<double>> sf;   // sf[c][k] = S_c(k), k = 0 .. T − 1 − c
  std::vector<std::vector<double>> init; // init[a][t] = S(t + 1 + a) / S(a)
};

inline Tables tables(std::size_t T, const std::vector<Lifetime>& laws, const Initial* initial) {
  Tables tb;
  tb.sf.resize(T);
  for (std::size_t c = 0; c < T; c++) tb.sf[c] = law_of(laws, c).survival(T - c);
  if (initial) {
    const std::size_t A = initial->by_age.size();
    const auto S = initial->law.survival(T + A + 1);
    tb.init.assign(A, std::vector<double>(T, 0.0));
    for (std::size_t a = 0; a < A; a++) {
      require(initial->by_age[a] >= 0, "an initial stock ≥ 0 at every age");
      if (initial->by_age[a] == 0) continue;
      require(S[a] > 0, "an initial stock at an age its law says no unit reaches");
      for (std::size_t t = 0; t < T; t++) tb.init[a][t] = S[t + 1 + a] / S[a];
    }
  }
  return tb;
}

inline void finish(Model& M, const Initial* initial) {
  const std::size_t T = M.inflow.size();
  M.stock.assign(T, 0.0), M.outflow.assign(T, 0.0), M.net_addition.assign(T, 0.0), M.mean_age.assign(T, 0.0);
  M.outflow_by_cohort.assign(T, {});
  M.initial_stock.assign(T, 0.0), M.initial_outflow.assign(T, 0.0);
  M.initial_total = 0;
  M.initial_start = initial ? initial->by_age : std::vector<double>{};
  for (double v : M.initial_start) M.initial_total += v;
  for (std::size_t t = 0; t < T; t++) {
    M.outflow_by_cohort[t].assign(t + 1, 0.0);
    double age_mass = 0;
    for (std::size_t c = 0; c <= t; c++) {
      const double s = M.by_cohort[t][c];
      M.stock[t] += s;
      age_mass += s * static_cast<double>(t - c);
      const double before = c == t ? M.inflow[c] : M.by_cohort[t - 1][c];
      M.outflow_by_cohort[t][c] = before - s;
      M.outflow[t] += before - s;
    }
    if (initial) {
      for (std::size_t a = 0; a < M.initial_by_age[t].size(); a++) {
        const double s = M.initial_by_age[t][a];
        M.initial_stock[t] += s;
        age_mass += s * static_cast<double>(t + 1 + a);
        const double before = t == 0 ? initial->by_age[a] : M.initial_by_age[t - 1][a];
        M.initial_outflow[t] += before - s;
      }
      M.stock[t] += M.initial_stock[t];
      M.outflow[t] += M.initial_outflow[t];
    }
    M.net_addition[t] = M.inflow[t] - M.outflow[t];
    M.mean_age[t] = M.stock[t] > 0 ? age_mass / M.stock[t] : 0.0;
    const double previous = t == 0 ? M.initial_total : M.stock[t - 1];
    M.balance = std::max(M.balance, std::fabs(M.stock[t] - previous - M.net_addition[t]));
  }
  if (M.retired_early.empty()) M.retired_early.assign(T, 0.0);
}

}  // namespace detail

inline Model inflow_driven(const std::vector<double>& inflow, const std::vector<Lifetime>& laws, const Initial* initial = nullptr) {
  const std::size_t T = inflow.size();
  const auto tb = detail::tables(T, laws, initial);
  Model M;
  M.inflow = inflow;
  M.by_cohort.assign(T, {});
  M.initial_by_age.assign(T, {});
  for (std::size_t t = 0; t < T; t++) {
    M.by_cohort[t].assign(t + 1, 0.0);
    for (std::size_t c = 0; c <= t; c++) M.by_cohort[t][c] = inflow[c] * tb.sf[c][t - c];
    if (initial) {
      M.initial_by_age[t].assign(initial->by_age.size(), 0.0);
      for (std::size_t a = 0; a < initial->by_age.size(); a++) M.initial_by_age[t][a] = initial->by_age[a] * (initial->by_age[a] > 0 ? tb.init[a][t] : 0.0);
    }
  }
  detail::finish(M, initial);
  return M;
}

inline Model stock_driven(const std::vector<double>& stock, const std::vector<Lifetime>& laws, const Initial* initial = nullptr, bool correct_negative = true) {
  const std::size_t T = stock.size();
  const auto tb = detail::tables(T, laws, initial);
  Model M;
  M.inflow.assign(T, 0.0);
  M.retired_early.assign(T, 0.0);
  M.by_cohort.assign(T, {});
  M.initial_by_age.assign(T, {});
  std::vector<double> scale(T, 1.0);  // each cohort's factor, after early retirements
  double initial_scale = 1;
  for (std::size_t t = 0; t < T; t++) {
    detail::require(stock[t] >= 0 && std::isfinite(stock[t]), "a stock ≥ 0 in every period");
    M.by_cohort[t].assign(t + 1, 0.0);
    double remaining = 0;
    for (std::size_t c = 0; c < t; c++) remaining += M.by_cohort[t][c] = M.inflow[c] * scale[c] * tb.sf[c][t - c];
    if (initial) {
      M.initial_by_age[t].assign(initial->by_age.size(), 0.0);
      for (std::size_t a = 0; a < initial->by_age.size(); a++)
        remaining += M.initial_by_age[t][a] = initial->by_age[a] * initial_scale * (initial->by_age[a] > 0 ? tb.init[a][t] : 0.0);
    }
    const double s0 = tb.sf[t][0];
    detail::require(s0 > 0, "a lifetime law under which a cohort survives its first period (S(0) > 0)");
    double I = (stock[t] - remaining) / s0;
    if (I < 0 && correct_negative) {
      I = 0;
      const double r = remaining > 0 ? stock[t] / remaining : 0.0;
      M.retired_early[t] = remaining - stock[t];
      for (std::size_t c = 0; c < t; c++) scale[c] *= r, M.by_cohort[t][c] *= r;
      initial_scale *= r;
      for (double& v : M.initial_by_age[t]) v *= r;
    }
    M.inflow[t] = I;
    M.by_cohort[t][t] = I * s0;
  }
  detail::finish(M, initial);
  return M;
}

}  // namespace industrial_ecology::stocks
