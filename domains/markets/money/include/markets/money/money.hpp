// Money in the foam: its round, the loop of a loan, and where new money goes first (theory note, Part III §1).
//
//   velocity   the payments of a record over the area of its money faces, V = Σ m_v / ∫ M dt: the coefficient of the
//              equation of exchange read as a count of exchange events, once by their money legs and once by their goods
//              legs (Proposition C.1; Fisher 1911)
//   fisher     the loop that sells goods for money at the log price ln P₀, holds a claim at the nominal rate i for a time τ,
//              buys goods back at ln P₁ and returns along a holding of goods at its own rate ρ has the holonomy
//              (i − π − ρ) τ, π = (ln P₁ − ln P₀)/τ. Its flatness is Fisher's relation i = ρ + π (Proposition C.3)
//   Economy    sites that each produce y per unit of time, hold money m and spend it at the rate v, site i's spending going to
//              site j in the share W_ij (rows summing to one). Receipts clear each site's market:
//                  p_j = v (Wᵀm)_j / y,      c_i = y m_i Σ_j W_ij / (Wᵀm)_j,      ṁ = v (Wᵀ − I) m.
//              Total consumption is total output at every time, whatever W is. At rest m is in proportion to the
//              stationary law of W: uniform, with one price, exactly when the columns of W also sum to one
//   injection  money ΔM added at one site of an economy at rest: the path of balances, prices and consumption, each site's
//              cumulative real gain ∫ (c_i − c_i at rest) dt, and the time at which each price has made half its rise
//   first_order_gains   those gains to first order in ΔM for W whose columns also sum to one:
//                  G = (ΔM/p) (I − W Wᵀ) (I − Wᵀ)⁺ (e₀ − 1/n),
//              which for symmetric W is (ΔM/p)(I + W)(e₀ − 1/n) (Proposition C.4): the site of the injection and those that
//              sell to it gain, every other site loses 2ΔM/(pn), and the gains add up to zero (Cantillon 1755)
//
// The source writes the receipts as (Wm)_j and assumes W symmetric; with Wᵀ the conservation of consumption needs no
// symmetry (theory note, amendment A4).
//
// Sources: W. Huang (2026), Cases v0.9b, §3. I. Fisher (1911), The Purchasing Power of Money; (1930), The Theory of Interest.

#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/expm.hpp>
#include <calx/linalg/rectangular.hpp>
#include <physis/statistical_mechanics/master_equation.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace markets::money {

inline double velocity(double payments, double money_area) {
  if (!(money_area > 0)) throw std::invalid_argument("markets::money::velocity: money stood for some time");
  return payments / money_area;
}

struct FisherLoop {
  double inflation = 0, holonomy = 0, real_factor = 1;  // the creditor's real position is multiplied by e^{holonomy} against the flat loop's
};
inline FisherLoop fisher(double log_price_0, double log_price_1, double nominal_rate, double own_rate, double duration) {
  if (!(duration > 0)) throw std::invalid_argument("markets::money::fisher: a loan lasts");
  FisherLoop L;
  L.inflation = (log_price_1 - log_price_0) / duration;
  L.holonomy = (nominal_rate - L.inflation - own_rate) * duration;
  L.real_factor = std::exp(L.holonomy);
  return L;
}

struct Economy {
  calx::linalg::Matrix W;  // W(i, j): the share of site i's spending that goes to site j
  double spending = 1;     // v, per unit of time
  double output = 1;       // y, per site and unit of time
};

inline void validate(const Economy& E) {
  if (E.W.n == 0 || !(E.spending > 0) || !(E.output > 0)) throw std::invalid_argument("markets::money::Economy: sites, a rate of spending and an output, positive");
  for (std::size_t i = 0; i < E.W.n; i++) {
    double row = 0;
    for (std::size_t j = 0; j < E.W.n; j++) {
      if (!(E.W(i, j) >= 0)) throw std::invalid_argument("markets::money::Economy: shares of spending are not negative");
      row += E.W(i, j);
    }
    if (std::fabs(row - 1) > 1e-9) throw std::invalid_argument("markets::money::Economy: each site's shares of spending sum to one");
  }
}

// Whether the columns of W also sum to one: the economy then rests at equal balances and one price.
inline bool balanced(const Economy& E, double tolerance = 1e-9) {
  for (std::size_t j = 0; j < E.W.n; j++) {
    double column = 0;
    for (std::size_t i = 0; i < E.W.n; i++) column += E.W(i, j);
    if (std::fabs(column - 1) > tolerance) return false;
  }
  return true;
}

inline std::vector<double> receipts_share(const Economy& E, const std::vector<double>& m) {  // (Wᵀm)_j
  std::vector<double> r(E.W.n, 0.0);
  for (std::size_t i = 0; i < E.W.n; i++)
    for (std::size_t j = 0; j < E.W.n; j++) r[j] += E.W(i, j) * m[i];
  return r;
}
inline std::vector<double> prices(const Economy& E, const std::vector<double>& m) {
  auto p = receipts_share(E, m);
  for (double& x : p) x *= E.spending / E.output;
  return p;
}
inline std::vector<double> consumption(const Economy& E, const std::vector<double>& m) {
  const auto r = receipts_share(E, m);
  std::vector<double> c(E.W.n, 0.0);
  for (std::size_t i = 0; i < E.W.n; i++) {
    for (std::size_t j = 0; j < E.W.n; j++)
      if (E.W(i, j) > 0) c[i] += E.W(i, j) / r[j];
    c[i] *= E.output * m[i];
  }
  return c;
}

// The balances at rest holding the total M: in proportion to the stationary law of W.
inline std::vector<double> rest(const Economy& E, double total) {
  validate(E);
  calx::linalg::Matrix k = E.W;  // W's off-diagonal shares as rates: their stationary law is W's
  for (std::size_t i = 0; i < k.n; i++) k(i, i) = 0;
  auto pi = physis::statistical_mechanics::stationary(k);
  for (double& x : pi) x *= total;
  return pi;
}

struct Injection {
  std::vector<double> time;                  // the instants sampled
  std::vector<std::vector<double>> balance, price, consumed;  // [instant][site]
  std::vector<double> gain;                  // ∫ (c_i − c_i at rest) dt over the horizon
  std::vector<double> half_rise;             // when each price has made half its way to its final value (NaN where it does not move)
  std::vector<double> price_before, price_after;
  double total_consumed_defect = 0;          // max over the instants of |Σ c − n y|
};

// Money `added` at `site` at time 0 to an economy at rest with the total `money`, followed to `horizon` in `steps` equal steps.
inline Injection injection(const Economy& E, double money, std::size_t site, double added, double horizon, std::size_t steps) {
  validate(E);
  const std::size_t n = E.W.n;
  if (site >= n || !(money > 0) || !(horizon > 0) || steps < 2 || steps % 2) throw std::invalid_argument("markets::money::injection: a site of the economy, money, a horizon, and an even number of steps");
  Injection R;
  auto m = rest(E, money);
  R.price_before = prices(E, m);
  const auto resting = consumption(E, m);
  R.price_after = prices(E, rest(E, money + added));
  m[site] += added;
  // ṁ = v (Wᵀ − I) m is linear: one step is the matrix e^{v (Wᵀ − I) h}.
  const double h = horizon / static_cast<double>(steps);
  calx::linalg::Matrix A(n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) A(i, j) = E.spending * h * (E.W(j, i) - (i == j ? 1.0 : 0.0));
  const auto step = calx::linalg::expm(A);
  R.gain.assign(n, 0.0);
  R.half_rise.assign(n, std::numeric_limits<double>::quiet_NaN());
  for (std::size_t k = 0; k <= steps; k++) {
    const auto p = prices(E, m);
    const auto c = consumption(E, m);
    double total = 0;
    for (double x : c) total += x;
    R.total_consumed_defect = std::max(R.total_consumed_defect, std::fabs(total - static_cast<double>(n) * E.output));
    const double simpson = (k == 0 || k == steps ? 1.0 : k % 2 ? 4.0 : 2.0) * h / 3;
    for (std::size_t i = 0; i < n; i++) {
      R.gain[i] += simpson * (c[i] - resting[i]);
      const double half = R.price_before[i] + 0.5 * (R.price_after[i] - R.price_before[i]);
      if (std::isnan(R.half_rise[i]) && R.price_after[i] > R.price_before[i] && p[i] >= half) {
        if (k == 0) R.half_rise[i] = 0;
        else {
          const double before = R.price.back()[i];
          R.half_rise[i] = h * (static_cast<double>(k - 1) + (p[i] > before ? (half - before) / (p[i] - before) : 1.0));
        }
      }
    }
    R.time.push_back(h * static_cast<double>(k));
    R.balance.push_back(m);
    R.price.push_back(p);
    R.consumed.push_back(c);
    std::vector<double> next(n, 0.0);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) next[i] += step(i, j) * m[j];
    m = std::move(next);
  }
  return R;
}

// The gains to first order in the money added, for W whose columns also sum to one; p is the price at rest.
inline std::vector<double> first_order_gains(const Economy& E, std::size_t site, double added, double price) {
  validate(E);
  const std::size_t n = E.W.n;
  if (!balanced(E)) throw std::invalid_argument("markets::money::first_order_gains: for shares of spending whose columns also sum to one (equal balances at rest)");
  if (site >= n || !(price > 0)) throw std::invalid_argument("markets::money::first_order_gains: a site of the economy and a price");
  // x solves (I − Wᵀ) x = e₀ − 1/n on the vectors summing to zero: add the projection onto the constants to make it regular.
  calx::linalg::Matrix A(n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) A(i, j) = (i == j ? 1.0 : 0.0) - E.W(j, i) + 1.0 / static_cast<double>(n);
  calx::linalg::Matrix rhs(n);
  std::vector<double> d(n, -1.0 / static_cast<double>(n));
  d[site] += 1;
  for (std::size_t i = 0; i < n; i++) rhs(i, 0) = d[i];
  const auto X = calx::linalg::solve(A, rhs);
  std::vector<double> x(n), wt(n, 0.0), g(n, 0.0);
  for (std::size_t i = 0; i < n; i++) x[i] = X(i, 0);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) wt[j] += E.W(i, j) * x[i];  // Wᵀx
  for (std::size_t i = 0; i < n; i++) {
    double wwt = 0;
    for (std::size_t j = 0; j < n; j++) wwt += E.W(i, j) * wt[j];   // W Wᵀ x
    g[i] = added / price * (x[i] - wwt);
  }
  return g;
}

// The same for symmetric W, in the closed form of the source: (ΔM/p)(I + W)(e₀ − 1/n).
inline std::vector<double> symmetric_gains(const Economy& E, std::size_t site, double added, double price) {
  const std::size_t n = E.W.n;
  std::vector<double> g(n);
  for (std::size_t i = 0; i < n; i++) g[i] = added / price * ((i == site ? 1.0 : 0.0) + E.W(i, site) - 2.0 / static_cast<double>(n));
  return g;
}

}  // namespace markets::money
