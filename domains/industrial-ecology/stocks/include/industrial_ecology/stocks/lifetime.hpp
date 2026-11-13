// The lifetime of goods in use: how long what enters a stock stays in it (theory note §2.1).
//
// A lifetime law is a survival function S(a) = P(T > a), the share of a cohort still in use at age a.
// A dynamic stock model reads it at whole ages, a = 0, 1, 2, …: a cohort that entered in period c holds
// I(c) S(t − c) at the end of period t. This is the convention of Müller (2006, Ecol. Econ. 59: 142–156)
// and of ODYM (Pauliuk and Heeren 2020, J. Ind. Ecol. 24: 446–458), so that a law with S(0) < 1 (a
// normal law not truncated at zero) loses a little of each cohort in the period it enters.
//
// The laws, each from tyche or peira:
//   fixed(L)                    S(a) = 1 for a < L, else 0: every unit leaves at age ⌈L⌉
//   exponential(mean)           S(a) = e^{−a/mean}: a constant hazard, a geometric survival at whole ages
//   normal(mean, sd)            S(a) = 1 − Φ((a − mean)/sd), not truncated (ODYM's "Normal")
//   weibull(shape, scale)       S(a) = exp(−(a/scale)^shape), the law most fitted to buildings' lives
//                               (e.g. Miatto et al. 2017, Resour. Conserv. Recycl. 122: 143–154)
//   lognormal(mean, sd)         by the mean and standard deviation of the lifetime itself
//   gamma(shape, scale)
//   table(ages, survival)       a step function, right-continuous, 1 before the first age: a life table
//   from_kaplan_meier(km)       peira's estimate from observed lives, censored ones among them
//
// A law is declared, never guessed: the domain holds no default lifetime for any good.

#pragma once

#include <peira/survival/survival.hpp>
#include <tyche/probability/distributions.hpp>

#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace industrial_ecology::stocks {

struct Lifetime {
  std::string law;                          // its name and parameters, for the record
  std::function<double(double)> survival_of;  // S(a) = P(T > a)
  double mean = std::numeric_limits<double>::quiet_NaN();

  // S at whole ages 0 .. horizon − 1.
  std::vector<double> survival(std::size_t horizon) const {
    std::vector<double> s(horizon);
    for (std::size_t a = 0; a < horizon; a++) s[a] = survival_of(static_cast<double>(a));
    return s;
  }
};

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("industrial_ecology::stocks: ") + what);
}
inline std::string number(double v) {
  std::string s = std::to_string(v);
  while (s.size() > 1 && s.back() == '0') s.pop_back();
  if (!s.empty() && s.back() == '.') s.pop_back();
  return s;
}
}  // namespace detail

inline Lifetime fixed(double years) {
  detail::require(years > 0 && std::isfinite(years), "a fixed lifetime > 0");
  return {"fixed(" + detail::number(years) + ")", [years](double a) { return a < years ? 1.0 : 0.0; }, std::ceil(years)};
}
inline Lifetime exponential(double mean) {
  detail::require(mean > 0 && std::isfinite(mean), "an exponential lifetime with a mean > 0");
  const tyche::probability::Exponential law{1 / mean};
  return {"exponential(mean " + detail::number(mean) + ")", [law](double a) { return law.survival(a); }, mean};
}
inline Lifetime normal(double mean, double sd) {
  const tyche::probability::Normal law{mean, sd};
  law.validate();
  return {"normal(" + detail::number(mean) + ", " + detail::number(sd) + ")", [law](double a) { return law.survival(a); }, mean};
}
inline Lifetime weibull(double shape, double scale) {
  const tyche::probability::Weibull law{shape, scale};
  law.validate();
  return {"weibull(shape " + detail::number(shape) + ", scale " + detail::number(scale) + ")", [law](double a) { return law.survival(a); }, law.mean()};
}
inline Lifetime lognormal(double mean, double sd) {
  const auto law = tyche::probability::LogNormal::with_moments(mean, sd);
  return {"lognormal(mean " + detail::number(mean) + ", sd " + detail::number(sd) + ")", [law](double a) { return law.survival(a); }, mean};
}
inline Lifetime gamma(double shape, double scale) {
  const tyche::probability::Gamma law{shape, scale};
  return {"gamma(shape " + detail::number(shape) + ", scale " + detail::number(scale) + ")", [law](double a) { return law.survival(a); }, law.mean()};
}
// A step function through (ages[k], survival[k]), ages increasing, survival non-increasing in [0, 1].
inline Lifetime table(std::vector<double> ages, std::vector<double> survival) {
  detail::require(ages.size() == survival.size() && !ages.empty(), "a table of lifetimes needs one survival per age");
  for (std::size_t k = 0; k < ages.size(); k++) {
    detail::require(survival[k] >= 0 && survival[k] <= 1, "survival in [0, 1]");
    if (k) detail::require(ages[k] > ages[k - 1] && survival[k] <= survival[k - 1], "ages increasing and survival non-increasing");
  }
  double mean = 0, prev_age = 0, prev_s = 1;  // ∫ S: exact for the step function, up to the last age
  for (std::size_t k = 0; k < ages.size(); k++) {
    mean += prev_s * (ages[k] - prev_age);
    prev_age = ages[k], prev_s = survival[k];
  }
  if (prev_s > 0) mean = std::numeric_limits<double>::quiet_NaN();  // the table does not reach the end of every life
  return {"table(" + std::to_string(ages.size()) + " ages)",
          [ages = std::move(ages), survival = std::move(survival)](double a) {
            double s = 1;
            for (std::size_t k = 0; k < ages.size() && ages[k] <= a; k++) s = survival[k];
            return s;
          },
          mean};
}
inline Lifetime from_kaplan_meier(const peira::survival::KaplanMeier& km) {
  Lifetime L = table(km.time, km.survival);
  L.law = "kaplan-meier(" + std::to_string(km.time.size()) + " event times)";
  return L;
}

}  // namespace industrial_ecology::stocks
