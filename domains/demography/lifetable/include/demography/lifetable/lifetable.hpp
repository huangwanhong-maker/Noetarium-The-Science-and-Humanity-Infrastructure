// The life table: a population's mortality as a cohort would live it (Graunt 1662; Halley 1693; Chiang 1984; Preston, Heuveline and
// Guillot 2001, Demography, ch. 3). The domain's theory note, §1–2, states and proves what follows.
//
// THE PERIOD TABLE. Age groups [x, x + n) with death rates m_x (deaths per person-year), and a_x the mean years lived in the group by
// those who die in it. The probability of dying in the group is
//   q_x = n m_x / (1 + (n − a_x) m_x),
// and from a radix l_0: d_x = l_x q_x, l_{x+n} = l_x − d_x, the person-years L_x = n l_{x+n} + a_x d_x, T_x = Σ_{y ≥ x} L_y and the
// expectation of life e_x = T_x / l_x. The last group is open: q = 1 and L = l / m. a_x defaults to n/2 (deaths spread evenly);
// constant_hazard_a gives the value exact for a hazard constant within the group, 1/m − n e^{−mn} / (1 − e^{−mn}), under which the
// table reproduces the exponential law exactly (e_x = 1/m, checked).
//
// RATES FROM A RECORD. death_rates reads m_x from the person-time and deaths that peira's Lexis diagram gives for a period (the
// occurrence–exposure rates, maximum likelihood for a hazard constant within each cell), and standardised_death_rate weights them by
// a standard population through peira's direct standardisation.
//
// DECOMPOSITION. Arriaga's (1984) decomposition of the difference e_0² − e_0¹ between two tables into the contributions of the age
// groups, each the direct effect of the group's mortality on the years lived in it and the indirect effect on the survivors; the
// contributions sum to the difference exactly (a telescoping sum; checked).

#pragma once

#include <peira/cohorts/cohorts.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace demography::lifetable {

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("demography::lifetable: ") + what);
}
}  // namespace detail

struct LifeTable {
  std::vector<double> x, n;   // the groups' starting ages and widths (the last width +∞)
  std::vector<double> m, a, q, l, d, L, T, e;
  double radix = 100000;
  std::size_t size() const { return x.size(); }
};

// a for a hazard m constant over a group of width n.
inline double constant_hazard_a(double m, double n) {
  detail::require(m >= 0 && n > 0, "m ≥ 0 and n > 0");
  const double h = m * n;
  if (h < 1e-6) return n / 2 - m * n * n / 12;  // the series for small m n
  return 1 / m - n * std::exp(-h) / (1 - std::exp(-h));
}

// The table from starting ages x (increasing; the last group open), death rates m, and a (empty: n/2, the open group 1/m).
inline LifeTable life_table(const std::vector<double>& x, const std::vector<double>& m, std::vector<double> a = {}, double radix = 100000) {
  const std::size_t k = x.size();
  detail::require(k >= 2 && m.size() == k && (a.empty() || a.size() == k), "two groups at least, a rate for each (and a for each, if given)");
  detail::require(radix > 0, "a radix > 0");
  LifeTable t;
  t.x = x, t.m = m, t.radix = radix;
  t.n.resize(k);
  for (std::size_t i = 0; i + 1 < k; i++) {
    t.n[i] = x[i + 1] - x[i];
    detail::require(t.n[i] > 0, "increasing ages");
  }
  t.n[k - 1] = std::numeric_limits<double>::infinity();
  detail::require(m[k - 1] > 0, "a positive rate in the open group");
  if (a.empty()) {
    a.resize(k);
    for (std::size_t i = 0; i + 1 < k; i++) a[i] = t.n[i] / 2;
  }
  a[k - 1] = 1 / m[k - 1];
  t.a = a;
  t.q.resize(k), t.l.resize(k), t.d.resize(k), t.L.resize(k), t.T.resize(k), t.e.resize(k);
  double l = radix;
  for (std::size_t i = 0; i < k; i++) {
    detail::require(m[i] >= 0, "rates ≥ 0");
    t.l[i] = l;
    if (i + 1 < k) {
      detail::require(a[i] >= 0 && a[i] <= t.n[i], "0 ≤ a ≤ n");
      t.q[i] = t.n[i] * m[i] / (1 + (t.n[i] - a[i]) * m[i]);
      detail::require(t.q[i] <= 1, "q ≤ 1 (a rate too high for its a)");
      t.d[i] = l * t.q[i];
      const double next = l - t.d[i];
      t.L[i] = t.n[i] * next + a[i] * t.d[i];
      l = next;
    } else {
      t.q[i] = 1, t.d[i] = l, t.L[i] = l / m[i];
    }
  }
  double T = 0;
  for (std::size_t i = k; i-- > 0;) {
    T += t.L[i];
    t.T[i] = T;
    t.e[i] = t.l[i] > 0 ? T / t.l[i] : 0;
  }
  return t;
}

// Death rates by age band for one period band of a Lexis diagram: deaths over person-years (bands without exposure get NaN).
inline std::vector<double> death_rates(const peira::cohorts::Lexis& lexis, std::size_t period) {
  std::vector<double> m;
  for (std::size_t a = 0; a < lexis.person_time.size(); a++) {
    const double T = lexis.person_time[a].at(period);
    m.push_back(T > 0 ? lexis.events[a][period] / T : std::numeric_limits<double>::quiet_NaN());
  }
  return m;
}

// The death rate of a population standardised to a standard population's structure (peira's direct standardisation).
inline peira::cohorts::Standardised standardised_death_rate(const std::vector<double>& deaths, const std::vector<double>& exposure, const std::vector<double>& standard,
                                                            double confidence = 0.95) {
  return peira::cohorts::direct(deaths, exposure, standard, confidence);
}

// Arriaga's contributions of each age group to e_0 of `two` minus e_0 of `one` (tables of the same groups).
inline std::vector<double> arriaga(const LifeTable& one, const LifeTable& two) {
  detail::require(one.size() == two.size(), "two tables of the same groups");
  const std::size_t k = one.size();
  for (std::size_t i = 0; i < k; i++) detail::require(one.x[i] == two.x[i], "two tables of the same groups");
  std::vector<double> c(k);
  const double l0 = one.l[0];
  for (std::size_t i = 0; i + 1 < k; i++)
    c[i] = one.l[i] / l0 * (two.L[i] / two.l[i] - one.L[i] / one.l[i]) + two.T[i + 1] / l0 * (one.l[i] / two.l[i] - one.l[i + 1] / two.l[i + 1]);
  c[k - 1] = one.l[k - 1] / l0 * (two.T[k - 1] / two.l[k - 1] - one.T[k - 1] / one.l[k - 1]);
  return c;
}

}  // namespace demography::lifetable
