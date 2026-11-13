// Ecosystem services on a grid of land cover: carbon stored, habitat quality under threats, the water a landscape yields, and the
// soil it loses (ADR 0109; the four models of InVEST that compute in closed form: Sharp et al. 2020, InVEST User's Guide, Natural
// Capital Project).
//
// A grid holds a land-cover class in each cell (−1 for no data), cells of a side in metres. Every coefficient (a class's carbon
// pools, its habitat value and sensitivity to each threat, its crop coefficient, a threat's weight and reach) is declared by the
// caller with its source: the domain holds none, and an estimate of a service is no measure of an ecosystem's condition or worth
// (the handbook's §1.1, §6.3).
//
//   carbon(grid, pools)                   the carbon stored in four pools (above ground, below ground, soil, dead matter), Mg C per
//                                         ha by class, times each cell's area
//   habitat_quality(grid, classes, threats)
//                                         InVEST's habitat quality: the degradation of a habitat cell x of class j,
//                                         D_x = Σ_r Σ_y (w_r / Σ w) r_y i_rxy β_x S_jr, each threat r of weight w_r and intensity r_y at
//                                         cell y decaying with distance, linearly, i = 1 − d/d_max (none beyond d_max), or
//                                         exponentially, i = exp(−2.99 d / d_max) (5% at d_max; taken to where it falls below 10⁻³);
//                                         the quality Q_x = H_j (1 − D^z / (D^z + k^z)), z = 2.5, k the half-saturation constant
//   water_yield(grid, P, ET0, AWC, kc, Z) the annual water yield Y = (1 − AET/P) P with Budyko's curve in Fu's form,
//                                         AET/P = 1 + PET/P − (1 + (PET/P)^ω)^{1/ω}, PET = K_c ET₀, ω = Z AWC/P + 1.25 (Zhang et al.
//                                         2004, Water Resour. Res. 40: W02502; Donohue et al. 2012, J. Hydrol. 436: 35–50); a class
//                                         with no vegetation evaporates min(K_c ET₀, P)
//   soil_loss(R, K, slope, length, C, P)  the revised universal soil loss equation A = R K L S C P, t/ha/yr (Renard et al. 1997, USDA
//                                         Agriculture Handbook 703), with L = (λ/22.13)^m, m = β/(1 + β), β = (sin θ/0.0896)/(3 sin^0.8 θ
//                                         + 0.56), and S = 10.8 sin θ + 0.03 below a 9% slope, 16.8 sin θ − 0.50 above (McCool et al.
//                                         1987, 1989, Trans. ASAE 30: 1387–1396, 32: 1571–1576)
//
// Accuracy: exact arithmetic on the declared coefficients; distances between cell centres.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::ecosystems {

struct Grid {
  std::size_t cols = 0, rows = 0;
  double cell = 30;              // m
  std::vector<int> cover;        // row-major, a class per cell, −1 where there is none
  double cell_ha() const { return cell * cell / 1e4; }
  void validate() const {
    if (cols * rows == 0 || cover.size() != cols * rows || !(cell > 0)) throw std::invalid_argument("ecology::ecosystems: a grid of cols × rows cells of a positive side, a class in each");
  }
};

// ---- carbon ----

struct Pools {
  double above = 0, below = 0, soil = 0, dead = 0;  // Mg C per ha
  double total() const { return above + below + soil + dead; }
};

struct Carbon {
  std::vector<double> per_cell;          // Mg C
  std::map<int, double> by_class;        // Mg C
  double total = 0, area_ha = 0;
  std::size_t unknown = 0;               // cells of a class with no pools declared
};

inline Carbon carbon(const Grid& g, const std::map<int, Pools>& pools) {
  g.validate();
  Carbon c;
  c.per_cell.assign(g.cover.size(), 0.0);
  for (std::size_t i = 0; i < g.cover.size(); i++) {
    if (g.cover[i] < 0) continue;
    c.area_ha += g.cell_ha();
    const auto it = pools.find(g.cover[i]);
    if (it == pools.end()) {
      c.unknown++;
      continue;
    }
    c.per_cell[i] = it->second.total() * g.cell_ha();
    c.by_class[g.cover[i]] += c.per_cell[i];
    c.total += c.per_cell[i];
  }
  return c;
}

// ---- habitat quality ----

struct Threat {
  std::string name;
  double weight = 1;
  double max_distance = 1000;     // m
  bool exponential = false;
  std::vector<double> intensity;  // per cell, in [0, 1]
};

struct HabitatClass {
  double habitat = 0;                // H_j in [0, 1]
  std::vector<double> sensitivity;   // S_jr per threat, in [0, 1]
};

struct Habitat {
  std::vector<double> degradation, quality;  // per cell (NaN where there is no class)
};

inline Habitat habitat_quality(const Grid& g, const std::map<int, HabitatClass>& classes, const std::vector<Threat>& threats, double half_saturation = 0.05,
                               double z = 2.5, const std::vector<double>& access = {}) {
  g.validate();
  const std::size_t N = g.cover.size();
  if (!access.empty() && access.size() != N) throw std::invalid_argument("ecology::ecosystems::habitat_quality: an accessibility per cell, or none");
  double wsum = 0;
  for (const auto& t : threats) {
    if (t.intensity.size() != N || !(t.weight >= 0) || !(t.max_distance > 0)) throw std::invalid_argument("ecology::ecosystems::habitat_quality: each threat has an intensity per cell, a weight ≥ 0 and a reach > 0");
    wsum += t.weight;
  }
  Habitat h;
  h.degradation.assign(N, NAN), h.quality.assign(N, NAN);
  for (std::size_t x = 0; x < N; x++) {
    if (g.cover[x] < 0) continue;
    const auto it = classes.find(g.cover[x]);
    if (it == classes.end()) throw std::invalid_argument("ecology::ecosystems::habitat_quality: a class with no habitat value declared: " + std::to_string(g.cover[x]));
    const HabitatClass& hc = it->second;
    if (hc.sensitivity.size() != threats.size()) throw std::invalid_argument("ecology::ecosystems::habitat_quality: one sensitivity per threat for every class");
    const double cx = static_cast<double>(x % g.cols), cy = static_cast<double>(x / g.cols);
    double D = 0;
    if (hc.habitat > 0 && wsum > 0)
      for (std::size_t r = 0; r < threats.size(); r++) {
        const Threat& t = threats[r];
        if (hc.sensitivity[r] == 0 || t.weight == 0) continue;
        const double reach = t.exponential ? t.max_distance * std::log(1000.0) / 2.99 : t.max_distance;  // to where it falls below 10⁻³
        const long span = static_cast<long>(std::ceil(reach / g.cell));
        double sum = 0;
        for (long dy = -span; dy <= span; dy++)
          for (long dx = -span; dx <= span; dx++) {
            const long yx = static_cast<long>(cx) + dx, yy = static_cast<long>(cy) + dy;
            if (yx < 0 || yy < 0 || yx >= static_cast<long>(g.cols) || yy >= static_cast<long>(g.rows)) continue;
            const double ry = t.intensity[static_cast<std::size_t>(yy) * g.cols + static_cast<std::size_t>(yx)];
            if (ry == 0) continue;
            const double d = g.cell * std::sqrt(static_cast<double>(dx * dx + dy * dy));
            const double i = t.exponential ? (d <= reach ? std::exp(-2.99 * d / t.max_distance) : 0.0) : (d < t.max_distance ? 1 - d / t.max_distance : 0.0);
            sum += ry * i;
          }
        D += t.weight / wsum * sum * hc.sensitivity[r];
      }
    const double beta = access.empty() ? 1.0 : access[x];
    D *= beta;
    h.degradation[x] = D;
    const double Dz = std::pow(D, z), kz = std::pow(half_saturation, z);
    h.quality[x] = hc.habitat * (1 - (Dz + kz > 0 ? Dz / (Dz + kz) : 0.0));
  }
  return h;
}

// ---- annual water yield ----

struct WaterClass {
  double kc = 1;          // crop coefficient
  bool vegetated = true;  // false: evaporation is min(K_c ET₀, P)
};

struct WaterYield {
  std::vector<double> yield_mm, aet_mm;  // per cell
  double volume_m3 = 0;                  // Σ yield × cell area
};

inline double fu_budyko(double pet_over_p, double omega) { return 1 + pet_over_p - std::pow(1 + std::pow(pet_over_p, omega), 1 / omega); }

inline WaterYield water_yield(const Grid& g, const std::vector<double>& P, const std::vector<double>& ET0, const std::vector<double>& AWC, const std::map<int, WaterClass>& classes, double Z) {
  g.validate();
  const std::size_t N = g.cover.size();
  if (P.size() != N || ET0.size() != N || AWC.size() != N) throw std::invalid_argument("ecology::ecosystems::water_yield: precipitation, reference evapotranspiration and available water per cell");
  WaterYield w;
  w.yield_mm.assign(N, NAN), w.aet_mm.assign(N, NAN);
  for (std::size_t x = 0; x < N; x++) {
    if (g.cover[x] < 0 || !(P[x] > 0)) continue;
    const auto it = classes.find(g.cover[x]);
    if (it == classes.end()) throw std::invalid_argument("ecology::ecosystems::water_yield: a class with no crop coefficient declared: " + std::to_string(g.cover[x]));
    const double pet = it->second.kc * ET0[x];
    double aet;
    if (it->second.vegetated) {
      const double omega = Z * AWC[x] / P[x] + 1.25;
      aet = P[x] * std::min(fu_budyko(pet / P[x], omega), 1.0);
    } else {
      aet = std::min(pet, P[x]);
    }
    w.aet_mm[x] = aet;
    w.yield_mm[x] = P[x] - aet;
    w.volume_m3 += w.yield_mm[x] / 1000 * g.cell * g.cell;
  }
  return w;
}

// ---- soil loss ----

inline double rusle_length(double slope_rad, double length_m) {
  const double s = std::sin(slope_rad);
  const double beta = (s / 0.0896) / (3 * std::pow(s, 0.8) + 0.56);
  const double m = beta / (1 + beta);
  return std::pow(length_m / 22.13, m);
}
inline double rusle_steepness(double slope_rad) {
  const double s = std::sin(slope_rad);
  return std::tan(slope_rad) < 0.09 ? 10.8 * s + 0.03 : 16.8 * s - 0.50;
}
// A = R K L S C P, t/ha/yr: R in MJ mm/(ha h yr), K in t ha h/(ha MJ mm).
inline double soil_loss(double R, double K, double slope_rad, double length_m, double C, double Pf) {
  return R * K * rusle_length(slope_rad, length_m) * rusle_steepness(slope_rad) * C * Pf;
}

}  // namespace ecology::ecosystems
