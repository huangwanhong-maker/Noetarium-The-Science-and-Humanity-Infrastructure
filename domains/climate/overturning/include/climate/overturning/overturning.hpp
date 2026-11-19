// The overturning of an ocean basin in two boxes (Stommel 1961, Tellus 13, 224–230), with Cessi's exchange law (Cessi 1994,
// J. Phys. Oceanogr. 24, 1911–1920), in the dimensionless form of Berglund and Gentz (arXiv:physics/0111110, §4, their
// eqs. 4.1–4.6, as verified by the survey *Critical transitions in structured systems*, Part III §2.2):
//
//     ΔT' = −(ΔT − θ)/τ_r − Q(Δρ) ΔT,   ΔS' = F/(H S₀) − Q(Δρ) ΔS,   Δρ = α_S ΔS − α_T ΔT,   Q(Δρ) = 1/τ_d + (q/V) Δρ²,
//
// and with y = α_S ΔS/(α_T θ), z = ΔT/θ, time in units of the diffusive time τ_d, ε₀ = τ_r/τ_d and η² = τ_d (α_T θ)² q/V,
//
//     y' = p(t) − y [1 + η² (y − z)²],   ε₀ z' = −(z − 1) − ε₀ z [1 + η² (y − z)²]           (4.5)
//
// which on its slow manifold z = 1 + O(ε₀) reduces to
//
//     y' = p(t) − y [1 + η² (y − 1)²]                                                       (4.6)
//
// p is the freshwater forcing. The reduced model's equilibria solve p = y[1 + η²(y − 1)²], a cubic; its folds are where dp/dy
// = 3η²y² − 4η²y + 1 + η² vanishes, y± = 2/3 ± √(η² − 3)/(3η), so it is bistable for η² > 3 (the cusp at η² = 3) between
// p(y₊) and p(y₋). Cessi's estimates η² ≈ 7.5, τ_r ≈ 25 days, τ_d ≈ 219 years (Berglund and Gentz's reading) give the
// interval [0.964, 1.480]; `cessi()` declares them. The restoring rate at an equilibrium is 1 + η²(y − 1)(3y − 1), in units
// of 1/τ_d, and vanishes at the folds: the overturning's loss as p rises past p(y₋) is a fold that slows before it.
#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace climate::overturning {

struct Cessi {
  double eta2 = 7.5;                 // η²
  double epsilon = 25.0 / (219.0 * 365.25);  // ε₀ = τ_r/τ_d
  double diffusive_years = 219;      // τ_d, the unit of time
};

inline Cessi cessi() { return {}; }

// The reduced law (4.6).
inline double tendency(const Cessi& c, double p, double y) { return p - y * (1 + c.eta2 * (y - 1) * (y - 1)); }

// The restoring rate −∂y'/∂y = 1 + η²(y − 1)(3y − 1).
inline double rate(const Cessi& c, double y) { return 1 + c.eta2 * (y - 1) * (3 * y - 1); }

// The forcing at which y is an equilibrium.
inline double forcing_at(const Cessi& c, double y) { return y * (1 + c.eta2 * (y - 1) * (y - 1)); }

struct Folds {
  double y_upper = 0, y_lower = 0;   // y₊ (the upper branch's end) and y₋ (the lower branch's end)
  double p_lower = 0, p_upper = 0;   // p(y₊) ≤ p(y₋): the bistable interval
};

inline Folds folds(const Cessi& c) {
  if (!(c.eta2 > 3)) throw std::invalid_argument("climate::overturning::folds: the reduced model has folds only for η² > 3");
  Folds f;
  const double s = std::sqrt(c.eta2 - 3) / (3 * std::sqrt(c.eta2));
  f.y_upper = 2.0 / 3 + s, f.y_lower = 2.0 / 3 - s;
  f.p_lower = forcing_at(c, f.y_upper), f.p_upper = forcing_at(c, f.y_lower);
  return f;
}

struct Equilibrium {
  double y = 0, rate = 0;
  bool stable() const { return rate > 0; }
};

// The real roots of η² y³ − 2η² y² + (1 + η²) y − p = 0, ascending, each with its restoring rate.
inline std::vector<Equilibrium> equilibria(const Cessi& c, double p) {
  const double a = c.eta2, b = -2 * c.eta2, cc = 1 + c.eta2, d = -p;
  // Depressed cubic t³ + P t + R = 0 with y = t − b/(3a).
  const double P = (3 * a * cc - b * b) / (3 * a * a);
  const double R = (2 * b * b * b - 9 * a * b * cc + 27 * a * a * d) / (27 * a * a * a);
  const double shift = -b / (3 * a);
  std::vector<double> roots;
  const double disc = R * R / 4 + P * P * P / 27;
  if (disc > 1e-15) {
    const double u = std::cbrt(-R / 2 + std::sqrt(disc)), v = std::cbrt(-R / 2 - std::sqrt(disc));
    roots.push_back(u + v + shift);
  } else {
    const double m = 2 * std::sqrt(-P / 3);
    const double phi = std::acos(std::clamp(3 * R / (P * m), -1.0, 1.0)) / 3;
    for (int k = 0; k < 3; k++) roots.push_back(m * std::cos(phi - 2 * std::numbers::pi * k / 3) + shift);
  }
  std::sort(roots.begin(), roots.end());
  std::vector<Equilibrium> out;
  for (double y : roots) {
    // One Newton step on the cubic to polish.
    const double g = tendency(c, p, y), dg = -rate(c, y);
    if (dg != 0) y -= g / dg;
    out.push_back({y, rate(c, y)});
  }
  return out;
}

// The slow–fast pair (4.5) stepped once by Euler's method over h (in units of τ_d).
inline void step(const Cessi& c, double p, double& y, double& z, double h) {
  const double q = 1 + c.eta2 * (y - z) * (y - z);
  const double dy = p - y * q, dz = (-(z - 1) - c.epsilon * z * q) / c.epsilon;
  y += h * dy;
  z += h * dz;
}

}  // namespace climate::overturning
