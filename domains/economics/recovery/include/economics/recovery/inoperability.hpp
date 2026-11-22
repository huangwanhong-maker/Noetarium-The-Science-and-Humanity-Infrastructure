// Indirect economic losses over a recovery, by the inoperability input–output model (theory note `theory/recovery-and-inoperability.md`;
// ADR 0158): how a disruption to some sectors, by lost capacity or lost demand, runs through the others and how long it lasts.
//
// Inoperability is the share of a sector's planned output not produced, q_i = (x̂_i − x̃_i) / x̂_i. From Leontief's x = A x + c it
// follows exactly that q = A* q + c*, with A* = X̂⁻¹ A X̂ (a*_ij = z_ij / x̂_i, the share of i's output that j uses) and c* the lost final
// demand over planned output (Santos and Haimes 2004): the static model q = (I − A*)⁻¹ c*. The dynamic model (Lian and Haimes 2006)
// steps q(t+1) = q(t) + K [A* q(t) + c*(t) − q(t)], each sector's resilience k_i set so that, alone, it recovers to a declared share of
// its first inoperability in a declared number of periods: k_i = (1 − r^{1/T_i}) / (1 − a*_ii), the discrete form of Lian and Haimes's
// ln(q(0)/q(T)) / (T (1 − a*_ii)). The step is stable: every row of A* sums to below one where final demand is positive, so the
// ∞-norm of I − K(I − A*) is below one for k_i in (0, 1].
//
// What a path costs: each sector's output lost, x̂_i/periods-a-year times Σ_t q_i(t); the part each sector would lose alone (A* nought)
// against the total, so that indirect losses are the difference; sectors ranked by inoperability and by loss, which differ. Over an
// unbounded horizon lost demand costs its Leontief total whatever the resilience (Proposition 4), which only sets when.

#pragma once

#include <economics/production/input_output.hpp>

#include <calx/linalg/dense.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::recovery {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("economics::recovery: " + what);
}
}  // namespace detail

struct Interdependency {
  std::vector<std::string> sectors;
  calx::linalg::Matrix a;                // A*: a*_ij = z_ij / x̂_i
  std::vector<double> output;            // x̂, a year's planned output
  double largest_row = 0;                // max_i Σ_j a*_ij, below one where every final demand is positive
};

inline Interdependency interdependency(const InputOutputTable& T) {
  const std::size_t n = T.n();
  Interdependency I;
  I.sectors = T.sectors, I.output = T.output(), I.a = calx::linalg::Matrix(n);
  for (std::size_t i = 0; i < n; i++) {
    detail::require(I.output[i] > 0, "every sector has positive output");
    double row = 0;
    for (std::size_t j = 0; j < n; j++) I.a(i, j) = T.z(i, j) / I.output[i], row += I.a(i, j);
    I.largest_row = std::max(I.largest_row, row);
  }
  return I;
}

// The static model: q = (I − A*)⁻¹ c*.
inline std::vector<double> static_inoperability(const Interdependency& I, const std::vector<double>& c) {
  const std::size_t n = I.sectors.size();
  detail::require(c.size() == n, "one perturbation per sector");
  const auto L = calx::linalg::solve(calx::linalg::Matrix::identity(n) - I.a, calx::linalg::Matrix::identity(n));
  std::vector<double> q(n, 0.0);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) q[i] += L(i, j) * c[j];
  return q;
}

// Resilience from recovery times: each sector alone recovers to `remaining` of its first inoperability in T_i periods.
inline std::vector<double> resilience(const Interdependency& I, const std::vector<double>& periods, double remaining = 0.01) {
  const std::size_t n = I.sectors.size();
  detail::require(periods.size() == n, "one recovery time per sector");
  detail::require(remaining > 0 && remaining < 1, "the share remaining lies in (0, 1)");
  std::vector<double> k(n);
  for (std::size_t i = 0; i < n; i++) {
    detail::require(periods[i] >= 1, "a recovery takes at least one period");
    k[i] = std::min(1.0, (1 - std::pow(remaining, 1.0 / periods[i])) / (1 - I.a(i, i)));
  }
  return k;
}

struct Path {
  std::vector<std::vector<double>> q;    // q[t][i], t = 0 … periods
  std::vector<double> loss, alone;       // output lost by sector: with the interdependencies, and each sector alone (A* nought)
  double total = 0, total_alone = 0;     // their sums; the indirect losses are total − total_alone
  std::vector<int> recovered;            // the first period from which a sector's inoperability stays below `threshold`, or −1
  std::vector<double> mean;              // each sector's mean inoperability over the path
};

namespace detail {
inline std::vector<std::vector<double>> step(const calx::linalg::Matrix& a, const std::vector<double>& q0, const std::vector<double>& k,
                                             const std::vector<std::vector<double>>& c, int periods) {
  const std::size_t n = q0.size();
  std::vector<std::vector<double>> q{q0};
  for (int t = 0; t < periods; t++) {
    const auto& p = q.back();
    std::vector<double> nx(n);
    for (std::size_t i = 0; i < n; i++) {
      double s = 0;
      for (std::size_t j = 0; j < n; j++) s += a(i, j) * p[j];
      const double ci = static_cast<std::size_t>(t) < c.size() ? c[t][i] : 0.0;
      nx[i] = p[i] + k[i] * (s + ci - p[i]);
    }
    q.push_back(std::move(nx));
  }
  return q;
}
}  // namespace detail

// The dynamic model from a first inoperability q0 (lost capacity), with lost final demand c*(t) for the periods given (none after).
inline Path recover(const Interdependency& I, const std::vector<double>& q0, const std::vector<double>& k, const std::vector<std::vector<double>>& c, int periods,
                    double periods_a_year, double threshold = 0.001) {
  const std::size_t n = I.sectors.size();
  detail::require(q0.size() == n && k.size() == n, "one first inoperability and one resilience per sector");
  detail::require(periods >= 1 && periods_a_year > 0, "a path has periods, and a year has periods");
  for (std::size_t i = 0; i < n; i++) detail::require(q0[i] >= 0 && q0[i] <= 1 && k[i] > 0 && k[i] <= 1, "an inoperability lies in [0, 1] and a resilience in (0, 1]");
  for (const auto& ct : c) detail::require(ct.size() == n, "one perturbation per sector each period");
  Path P;
  P.q = detail::step(I.a, q0, k, c, periods);
  const auto alone = detail::step(calx::linalg::Matrix(n), q0, k, c, periods);
  P.loss.assign(n, 0.0), P.alone.assign(n, 0.0), P.mean.assign(n, 0.0), P.recovered.assign(n, -1);
  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t t = 0; t < P.q.size(); t++) {
      P.loss[i] += std::max(0.0, P.q[t][i]) * I.output[i] / periods_a_year;
      P.alone[i] += std::max(0.0, alone[t][i]) * I.output[i] / periods_a_year;
      P.mean[i] += P.q[t][i] / static_cast<double>(P.q.size());
    }
    for (int t = static_cast<int>(P.q.size()) - 1; t >= 0; t--) {
      if (P.q[t][i] >= threshold) break;
      P.recovered[i] = t;
    }
    P.total += P.loss[i], P.total_alone += P.alone[i];
  }
  return P;
}

// Over an unbounded horizon the inoperabilities sum to S = (I − A*)⁻¹ (C + K⁻¹ q0), C the perturbations' sum (Proposition 4: sum the
// step from 0 to ∞). Lost demand costs its Leontief total whatever the resilience, which sets only when; lost capacity costs more the
// slower each sector recovers. Each sector's output lost: x̂_i / periods-a-year times S_i.
inline std::vector<double> unbounded_loss(const Interdependency& I, const std::vector<double>& q0, const std::vector<double>& k, const std::vector<double>& c_sum,
                                          double periods_a_year) {
  const std::size_t n = I.sectors.size();
  detail::require(q0.size() == n && k.size() == n && c_sum.size() == n, "one entry per sector");
  std::vector<double> rhs(n);
  for (std::size_t i = 0; i < n; i++) {
    detail::require(k[i] > 0, "a resilience is positive");
    rhs[i] = c_sum[i] + q0[i] / k[i];
  }
  const auto S = static_inoperability(I, rhs);
  std::vector<double> loss(n);
  for (std::size_t i = 0; i < n; i++) loss[i] = S[i] * I.output[i] / periods_a_year;
  return loss;
}

// Sectors ordered by a measure, greatest first.
inline std::vector<std::size_t> ranking(const std::vector<double>& v) {
  std::vector<std::size_t> o(v.size());
  std::iota(o.begin(), o.end(), std::size_t{0});
  std::stable_sort(o.begin(), o.end(), [&](std::size_t a, std::size_t b) { return v[a] > v[b]; });
  return o;
}

}  // namespace economics::recovery
