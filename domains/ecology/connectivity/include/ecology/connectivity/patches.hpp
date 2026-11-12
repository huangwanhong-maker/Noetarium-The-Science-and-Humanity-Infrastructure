// Habitat as a network of patches: how much of a landscape's habitat is reachable from habitat, and what each patch does for
// that, by the integral index of connectivity and the probability of connectivity.
//
// Patches i carry an attribute a_i (their area, or area weighted by quality) in a landscape of area A_L. Between two patches
// lies a distance d_ij (edge to edge, or the cost of the least-cost path, as whoever composes them chooses).
//
//   IIC = Σ_i Σ_j a_i a_j / (1 + nl_ij) / A_L²       nl_ij the number of links on the shortest path between i and j in the
//                                                     graph that links patches within a threshold distance; a pair not
//                                                     connected contributes 0, and nl_ii = 0
//                                                     (Pascual-Hortal and Saura 2006, Landsc. Ecol. 21: 959–967, eq. 3)
//   PC  = Σ_i Σ_j a_i a_j p*_ij / A_L²               p_ij = exp(−k d_ij) the probability of a direct movement, with k so
//                                                     that p = 1/2 at a stated median distance; p*_ij the greatest product
//                                                     of p along any path between them, p*_ii = 1
//                                                     (Saura and Pascual-Hortal 2007, Landsc. Urban Plan. 83: 91–103, eq. 2)
//   ECA = √(A_L² PC)                                  the equivalent connected area: the area of one patch with the same
//                                                     reachable habitat (Saura et al. 2011, Ecol. Indic. 11: 407–416)
//
// p*_ij is a shortest path in weights −ln p_ij = k d_ij, found by morphe's Dijkstra. The importance of patch m is the
// proportional loss when it is removed, dPC_m = 100 (PC − PC_{−m}) / PC, and it splits into three parts (Saura and Rubio 2010,
// Ecography 33: 523–537):
//
//   intra_m = 100 a_m² / Σ a_i a_j p*_ij                     the habitat within the patch
//   flux_m  = 100 · 2 Σ_{j≠m} a_m a_j p*_mj / Σ a_i a_j p*_ij  the movement that starts or ends in it
//   connector_m = dPC_m − intra_m − flux_m ≥ 0              the movement between other patches that passes through it,
//                                                           zero for a patch on no best path between two others
#pragma once

#include <morphe/combinatorics/shortest_paths.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace ecology::connectivity {

using Distances = std::vector<std::vector<double>>;  // symmetric, d_ii = 0; +∞ for no direct movement

struct PatchImportance {
  double total = 0, intra = 0, flux = 0, connector = 0;  // dPC and its parts, percent
};

struct Connectivity {
  double pc = 0, eca = 0, iic = 0;
  std::vector<PatchImportance> importance;  // dPC for each patch
  std::vector<double> d_iic;                // dIIC for each patch, percent
};

namespace detail {

inline void validate(const std::vector<double>& a, const Distances& d) {
  if (d.size() != a.size()) throw std::invalid_argument("ecology::connectivity: one row of distances per patch");
  for (std::size_t i = 0; i < a.size(); i++) {
    if (!(a[i] >= 0)) throw std::invalid_argument("ecology::connectivity: attributes are non-negative");
    if (d[i].size() != a.size()) throw std::invalid_argument("ecology::connectivity: a square matrix of distances");
  }
}

// Σ_i Σ_j a_i a_j p*_ij over the patches not removed (removed = none for all).
inline double pc_numerator(const std::vector<double>& a, const Distances& d, double k, std::size_t removed, std::vector<std::vector<double>>* best = nullptr) {
  const std::size_t n = a.size();
  morphe::combinatorics::WeightedAdjacency g(n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++)
      if (i != j && i != removed && j != removed && std::isfinite(d[i][j])) g[i].push_back({j, k * d[i][j]});
  double num = 0;
  if (best) best->assign(n, std::vector<double>(n, 0.0));
  for (std::size_t i = 0; i < n; i++) {
    if (i == removed) continue;
    const auto sp = morphe::combinatorics::dijkstra(g, i);
    for (std::size_t j = 0; j < n; j++) {
      if (j == removed || !std::isfinite(sp.distance[j])) continue;
      const double p = std::exp(-sp.distance[j]);
      num += a[i] * a[j] * p;
      if (best) (*best)[i][j] = p;
    }
  }
  return num;
}

inline double iic_numerator(const std::vector<double>& a, const Distances& d, double threshold, std::size_t removed) {
  const std::size_t n = a.size();
  morphe::combinatorics::WeightedAdjacency g(n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++)
      if (i != j && i != removed && j != removed && d[i][j] <= threshold) g[i].push_back({j, 1.0});
  double num = 0;
  for (std::size_t i = 0; i < n; i++) {
    if (i == removed) continue;
    const auto sp = morphe::combinatorics::dijkstra(g, i);
    for (std::size_t j = 0; j < n; j++)
      if (j != removed && std::isfinite(sp.distance[j])) num += a[i] * a[j] / (1 + sp.distance[j]);
  }
  return num;
}

}  // namespace detail

// PC and IIC with each patch's importance; `median_distance` where a direct movement has probability 1/2, `threshold` the
// distance within which two patches are linked for IIC.
inline Connectivity connectivity(const std::vector<double>& a, const Distances& d, double median_distance, double threshold, double landscape_area) {
  detail::validate(a, d);
  if (!(median_distance > 0) || !(landscape_area > 0)) throw std::invalid_argument("ecology::connectivity: a positive median distance and landscape area");
  const std::size_t n = a.size(), none = std::numeric_limits<std::size_t>::max();
  const double k = std::log(2.0) / median_distance, A2 = landscape_area * landscape_area;
  Connectivity c;
  std::vector<std::vector<double>> best;
  const double num = detail::pc_numerator(a, d, k, none, &best);
  const double inum = detail::iic_numerator(a, d, threshold, none);
  c.pc = num / A2;
  c.eca = std::sqrt(num);
  c.iic = inum / A2;
  c.importance.resize(n);
  c.d_iic.resize(n);
  for (std::size_t m = 0; m < n; m++) {
    auto& im = c.importance[m];
    if (num > 0) {
      im.total = 100 * (num - detail::pc_numerator(a, d, k, m)) / num;
      im.intra = 100 * a[m] * a[m] / num;
      double fl = 0;
      for (std::size_t j = 0; j < n; j++)
        if (j != m) fl += a[m] * a[j] * best[m][j];
      im.flux = 100 * 2 * fl / num;
      im.connector = im.total - im.intra - im.flux;
    }
    c.d_iic[m] = inum > 0 ? 100 * (inum - detail::iic_numerator(a, d, threshold, m)) / inum : 0;
  }
  return c;
}

}  // namespace ecology::connectivity
