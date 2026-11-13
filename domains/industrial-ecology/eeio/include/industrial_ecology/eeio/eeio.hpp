// Environmentally extended input–output analysis: the pressures of production attributed to the final
// demand that causes them, along supply chains and across regions (theory note §5; Miller and Blair
// 2009, Input–Output Analysis, 2nd ed., ch. 10 and 3; Kitzes 2013, Resources 2: 489–503).
//
// A multi-regional table has n sectors in each of R regions, sector s of region r at index r n + s
// (one region is the national case). Deliveries Z, final demand Y (one column per region: what it
// consumes, from anywhere), and an extension F (stressors × sectors: what each sector's production
// emits or extracts) with F_Y (stressors × regions: what final users emit directly, a household's
// boiler). With x = Z 1 + Y 1 the gross output, economics' Leontief model gives A = Z diag(x)⁻¹ and
// L = (I − A)⁻¹. Then
//
//   S = F diag(x)⁻¹                   direct intensities, pressure per unit of output
//   M = S L                           multipliers: pressure along every chain per unit of final demand
//   x^{(r)} = L Y_r                   the output, everywhere, that region r's final demand requires
//   P_k(ρ, r) = Σ_{j ∈ ρ} S_kj x^{(r)}_j  (+ F_Y(k, r) where ρ = r)
//
// P_k is the origin–destination matrix of stressor k: row sums are the production-based accounts (what
// is emitted in ρ), column sums the consumption-based accounts or footprints (what r's consumption
// causes, wherever), off-diagonal row sums what is embodied in exports, column sums in imports, and
//
//   production(r) − consumption(r) = exports embodied(r) − imports embodied(r)
//
// for every region (Peters 2008, Ecol. Econ. 65: 13–23; Wiedmann 2009, Ecol. Econ. 69: 211–222). These
// are pymrio's D_pba, D_cba, D_exp and D_imp summed by region (Stadler 2021, J. Open Res. Softw. 9: 8).
// With domestic extraction as the extension the footprint is the raw material consumption (the
// material footprint, Wiedmann et al. 2015, PNAS 112: 6271–6276).
//
//   characterise(ext, C)    stressors weighed into categories (e.g. greenhouse gases into CO₂-eq)
//   paths(…)                the leading supply chains of a region's footprint, by calx's walks

#pragma once

#include <economics/production/input_output.hpp>

#include <calx/linalg/dense.hpp>
#include <calx/linalg/nonnegative.hpp>
#include <calx/linalg/rectangular.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::eeio {

using calx::linalg::Matrix;
using calx::linalg::Rectangular;

struct Extension {
  std::vector<std::string> stressors, units;
  Rectangular F;    // stressors × sectors (all regions)
  Rectangular F_y;  // stressors × regions; zero columns where final users emit nothing directly
};

struct Mrio {
  std::vector<std::string> regions, sectors;  // R regions, n sectors in each
  Matrix Z;                                   // (R n) × (R n)
  Rectangular Y;                              // (R n) × R
  Extension ext;

  std::size_t n() const { return sectors.size(); }
  std::size_t R() const { return regions.size(); }
  std::size_t size() const { return n() * R(); }
  std::size_t region_of(std::size_t j) const { return j / n(); }
  std::string label(std::size_t j) const { return regions[region_of(j)] + " · " + sectors[j % n()]; }
};

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("industrial_ecology::eeio: " + what);
}
}  // namespace detail

struct Accounts {
  std::vector<double> output;                 // x
  economics::LeontiefModel leontief;          // A, L, and whether the economy is productive
  Rectangular S, M;                           // stressors × sectors
  std::vector<Matrix> origin;                 // per stressor, R × R: P_k(ρ, r)
  Rectangular production, consumption;        // stressors × regions
  Rectangular exports_embodied, imports_embodied;
  double balance = 0;                         // max over stressors and regions of |production − consumption − (exports − imports)|
};

inline Accounts accounts(const Mrio& io) {
  const std::size_t N = io.size(), R = io.R(), K = io.ext.stressors.size();
  detail::require(N > 0 && io.Z.n == N && io.Y.rows == N && io.Y.cols == R, "Z is (R n) square and Y is (R n) × R");
  detail::require(io.ext.F.rows == K && io.ext.F.cols == N, "F is stressors × sectors");
  detail::require(io.ext.F_y.cols == 0 || (io.ext.F_y.rows == K && io.ext.F_y.cols == R), "F_Y is stressors × regions, or empty");
  // Economics' table: final demand summed over regions, value added the balancing column.
  economics::InputOutputTable T;
  T.sectors.resize(N);
  for (std::size_t j = 0; j < N; j++) T.sectors[j] = io.label(j);
  T.z = io.Z;
  T.final_demand.assign(N, 0.0);
  for (std::size_t i = 0; i < N; i++)
    for (std::size_t r = 0; r < R; r++) T.final_demand[i] += io.Y(i, r);
  const auto x = T.output();
  T.value_added.assign(N, 0.0);
  for (std::size_t j = 0; j < N; j++) {
    double col = 0;
    for (std::size_t i = 0; i < N; i++) col += io.Z(i, j);
    T.value_added[j] = x[j] - col;
  }
  Accounts acc;
  acc.leontief = economics::leontief(T);
  acc.output = acc.leontief.output;
  const Matrix& L = acc.leontief.L;
  acc.S = Rectangular(K, N);
  for (std::size_t k = 0; k < K; k++)
    for (std::size_t j = 0; j < N; j++) acc.S(k, j) = io.ext.F(k, j) / acc.output[j];
  acc.M = Rectangular(K, N);
  for (std::size_t k = 0; k < K; k++)
    for (std::size_t i = 0; i < N; i++) {
      const double s = acc.S(k, i);
      if (s == 0) continue;
      for (std::size_t j = 0; j < N; j++) acc.M(k, j) += s * L(i, j);
    }
  // x^{(r)} = L Y_r.
  Rectangular X(N, R);
  for (std::size_t i = 0; i < N; i++)
    for (std::size_t l = 0; l < N; l++) {
      const double v = L(i, l);
      if (v == 0) continue;
      for (std::size_t r = 0; r < R; r++) X(i, r) += v * io.Y(l, r);
    }
  acc.origin.assign(K, Matrix(R));
  acc.production = Rectangular(K, R), acc.consumption = Rectangular(K, R);
  acc.exports_embodied = Rectangular(K, R), acc.imports_embodied = Rectangular(K, R);
  for (std::size_t k = 0; k < K; k++) {
    Matrix& P = acc.origin[k];
    for (std::size_t j = 0; j < N; j++) {
      const std::size_t rho = io.region_of(j);
      for (std::size_t r = 0; r < R; r++) P(rho, r) += acc.S(k, j) * X(j, r);
    }
    if (io.ext.F_y.cols)
      for (std::size_t r = 0; r < R; r++) P(r, r) += io.ext.F_y(k, r);
    for (std::size_t a = 0; a < R; a++)
      for (std::size_t b = 0; b < R; b++) {
        acc.production(k, a) += P(a, b);
        acc.consumption(k, b) += P(a, b);
        if (a != b) acc.exports_embodied(k, a) += P(a, b), acc.imports_embodied(k, b) += P(a, b);
      }
    for (std::size_t r = 0; r < R; r++)
      acc.balance = std::max(acc.balance, std::fabs(acc.production(k, r) - acc.consumption(k, r) - (acc.exports_embodied(k, r) - acc.imports_embodied(k, r))));
  }
  return acc;
}

// A region's footprint by the product its final demand bought (wherever made): stressors × n.
inline Rectangular by_product(const Mrio& io, const Accounts& acc, std::size_t r) {
  const std::size_t N = io.size(), n = io.n(), K = io.ext.stressors.size();
  detail::require(r < io.R(), "by_product: a region");
  Rectangular B(K, n);
  for (std::size_t k = 0; k < K; k++)
    for (std::size_t j = 0; j < N; j++) B(k, j % n) += acc.M(k, j) * io.Y(j, r);
  return B;
}

// Stressors weighed into categories: C is categories × stressors.
inline Extension characterise(const Extension& ext, const Rectangular& C, const std::vector<std::string>& categories, const std::vector<std::string>& units = {}) {
  detail::require(C.cols == ext.stressors.size() && C.rows == categories.size(), "characterise: C is categories × stressors");
  Extension out;
  out.stressors = categories;
  out.units = units;
  out.F = C * ext.F;
  if (ext.F_y.cols) out.F_y = C * ext.F_y;
  return out;
}

// The leading supply chains of region r's footprint in stressor k: walks from a product of final demand
// back through its suppliers, each walk's value the pressure where it ends.
inline calx::linalg::Walks paths(const Mrio& io, const Accounts& acc, std::size_t k, std::size_t r, double cutoff = 1e-3, std::size_t depth = 10, std::size_t keep = 30) {
  detail::require(k < io.ext.stressors.size() && r < io.R(), "paths: a stressor and a region");
  const std::size_t N = io.size();
  std::vector<double> e(N), y(N);
  for (std::size_t j = 0; j < N; j++) e[j] = acc.S(k, j), y[j] = io.Y(j, r);
  return calx::linalg::leading_walks(acc.leontief.A, e, y, cutoff, depth, keep);
}

}  // namespace industrial_ecology::eeio
