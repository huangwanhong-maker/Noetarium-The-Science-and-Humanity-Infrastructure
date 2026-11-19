// The planet's energy balance in one temperature, with the ice–albedo feedback that gives it two climates (Budyko 1969,
// Tellus 21, 611–619; Sellers 1969, J. Appl. Meteor. 8, 392–400; North, Cahalan and Coakley 1981, Rev. Geophys. 19, 91–121):
//
//     C dT/dt = Q (1 − α(T)) − (A + B T),
//
// T the mean surface temperature in °C, Q the mean insolation (a quarter of the solar constant), A + BT the outgoing
// longwave radiation fitted as a line, C the heat capacity of the layer that warms, and α(T) the planetary albedo, high
// where the surface is frozen and low where it is not. The albedo takes one of two forms:
//
//   ramp     α = α_i for T ≤ T_i, α = α_o for T ≥ T_o, linear between. The branches with constant albedo have equilibria
//            T* = (Q(1 − α) − A)/B and the restoring rate B/C exactly, until the equilibrium meets the ramp: the cold
//            branch exists while Q ≤ (A + B T_i)/(1 − α_i), the warm while Q ≥ (A + B T_o)/(1 − α_o), and the equilibrium on
//            the ramp is unstable when Q (α_i − α_o)/(T_o − T_i) > B (the survey *Critical transitions in structured
//            systems*, Part III §2.1, derived there). Its folds are non-smooth: nothing slows before them. It is the
//            negative case a detector of slowing down must not warn on.
//   smooth   α = α_i − (α_i − α_o) ½ (1 + tanh((T − T_c)/w)). Its folds are smooth, where Q(T) = (A + BT)/(1 − α(T)) has an
//            extremum, and the restoring rate goes to zero at them.
//
// The values a balance is given are the caller's. `common()` gives a set in common use, A = 203.3 W m⁻², B = 2.09
// W m⁻² K⁻¹ (North et al. 1981, as the survey records them from memory, to be checked against the paper), α_i = 0.62 and
// α_o = 0.30, and a heat capacity of 10 W yr m⁻² K⁻¹ (a mixed layer of roughly 75 m), declared and no measurement's; time is
// then in years.
#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace climate::energy_balance {

enum class Form { ramp, smooth };

struct Albedo {
  Form form = Form::ramp;
  double ice = 0.62, open = 0.30;     // α_i, α_o
  double cold = -10, warm = 0;        // ramp: T_i, T_o (°C)
  double centre = -5, width = 4;      // smooth: T_c, w (°C)
};

struct Balance {
  double A = 203.3, B = 2.09;         // W m⁻², W m⁻² K⁻¹
  double heat_capacity = 10;          // W yr m⁻² K⁻¹
  Albedo albedo;
};

inline Balance common(Form form = Form::ramp) {
  Balance b;
  b.albedo.form = form;
  return b;
}

inline double albedo(const Albedo& a, double T) {
  if (a.form == Form::ramp) {
    if (T <= a.cold) return a.ice;
    if (T >= a.warm) return a.open;
    return a.ice + (a.open - a.ice) * (T - a.cold) / (a.warm - a.cold);
  }
  return a.ice - (a.ice - a.open) * 0.5 * (1 + std::tanh((T - a.centre) / a.width));
}

inline double albedo_slope(const Albedo& a, double T) {
  if (a.form == Form::ramp) return T > a.cold && T < a.warm ? (a.open - a.ice) / (a.warm - a.cold) : 0.0;
  const double s = 1 / std::cosh((T - a.centre) / a.width);
  return -(a.ice - a.open) * 0.5 * s * s / a.width;
}

// dT/dt at insolation Q.
inline double tendency(const Balance& b, double Q, double T) { return (Q * (1 - albedo(b.albedo, T)) - (b.A + b.B * T)) / b.heat_capacity; }

// The restoring rate −∂(dT/dt)/∂T at T: (B + Q α'(T)) / C.
inline double rate(const Balance& b, double Q, double T) { return (b.B + Q * albedo_slope(b.albedo, T)) / b.heat_capacity; }

// The insolation at which T is an equilibrium, Q(T) = (A + BT)/(1 − α(T)).
inline double insolation_at(const Balance& b, double T) { return (b.A + b.B * T) / (1 - albedo(b.albedo, T)); }

struct Equilibrium {
  double T = 0;
  double rate = 0;   // > 0 stable
  bool stable() const { return rate > 0; }
};

// Every equilibrium in [lo, hi] °C at insolation Q, by bisection between sign changes on a fine grid.
inline std::vector<Equilibrium> equilibria(const Balance& b, double Q, double lo = -120, double hi = 80) {
  std::vector<Equilibrium> out;
  const int N = 20000;
  double t0 = lo, f0 = tendency(b, Q, t0);
  for (int i = 1; i <= N; i++) {
    const double t1 = lo + (hi - lo) * i / N, f1 = tendency(b, Q, t1);
    if (f0 == 0) out.push_back({t0, rate(b, Q, t0)});
    else if (f0 * f1 < 0) {
      double a = t0, c = t1, fa = f0;
      for (int k = 0; k < 80; k++) {
        const double m = 0.5 * (a + c), fm = tendency(b, Q, m);
        if (fa * fm <= 0) c = m;
        else a = m, fa = fm;
      }
      const double T = 0.5 * (a + c);
      out.push_back({T, rate(b, Q, T)});
    }
    t0 = t1, f0 = f1;
  }
  return out;
}

// The two folds: the insolation below which the warm climate is lost, and above which the cold one is.
struct Folds {
  double warm_lost = 0, T_warm = 0;   // the smallest Q with a warm equilibrium, and its temperature
  double cold_lost = 0, T_cold = 0;   // the largest Q with a cold equilibrium, and its temperature
};

inline Folds folds(const Balance& b) {
  Folds f;
  const Albedo& a = b.albedo;
  if (a.form == Form::ramp) {
    f.warm_lost = (b.A + b.B * a.warm) / (1 - a.open), f.T_warm = a.warm;
    f.cold_lost = (b.A + b.B * a.cold) / (1 - a.ice), f.T_cold = a.cold;
    return f;
  }
  // Smooth: Q(T) has a local maximum (the cold branch's end) and a local minimum (the warm branch's end) about T_c.
  auto extremum = [&](double lo, double hi, bool maximum) {
    double best = maximum ? -1e300 : 1e300, at = lo;
    for (int i = 0; i <= 4000; i++) {
      const double T = lo + (hi - lo) * i / 4000.0, q = insolation_at(b, T);
      if (maximum ? q > best : q < best) best = q, at = T;
    }
    // Golden-section refinement about the grid's best.
    double l = at - (hi - lo) / 4000.0, r = at + (hi - lo) / 4000.0;
    const double g = (std::sqrt(5.0) - 1) / 2;
    for (int k = 0; k < 100; k++) {
      const double m1 = r - g * (r - l), m2 = l + g * (r - l);
      const double q1 = insolation_at(b, m1), q2 = insolation_at(b, m2);
      if (maximum ? q1 > q2 : q1 < q2) r = m2;
      else l = m1;
    }
    return std::pair{insolation_at(b, 0.5 * (l + r)), 0.5 * (l + r)};
  };
  const auto [qmax, tmax] = extremum(a.centre - 6 * a.width, a.centre, true);
  const auto [qmin, tmin] = extremum(a.centre, a.centre + 6 * a.width, false);
  f.cold_lost = qmax, f.T_cold = tmax;
  f.warm_lost = qmin, f.T_warm = tmin;
  return f;
}

}  // namespace climate::energy_balance
