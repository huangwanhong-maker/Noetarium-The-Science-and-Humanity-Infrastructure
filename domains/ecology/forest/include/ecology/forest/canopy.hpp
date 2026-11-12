// Crowns on a grid: the fraction of each cell of a regular grid under at least one crown, the crowns as discs.
//
// A canopy's cover is the area of the union of its crowns, not the sum of their areas: crowns overlap along a street. Each
// cell is sampled at the centres of an s × s lattice of subcells and the fraction of them under a disc is its cover; the
// error in a cell is at most the area of the subcells the discs' boundaries cross, O(perimeter × cell/s), and vanishes as s
// grows (a midpoint rule for the indicator of the union). A disc of radius 0 covers nothing.
//
// The cover of a set of cells weighted by what lives in them (people, from a population grid) gives the canopy that people
// live under: its distribution and the share of people below a threshold are what an urban forest's equity asks of it.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace ecology::forest {

struct Grid {
  double x0 = 0, y0 = 0, cell = 1;  // the lower-left corner and the side of a cell, metres
  std::size_t nx = 0, ny = 0;
  std::size_t index(std::size_t i, std::size_t j) const { return j * nx + i; }
};

struct Disc {
  double x = 0, y = 0, r = 0;
};

inline std::vector<double> crown_cover(const Grid& g, const std::vector<Disc>& discs, std::size_t s = 4) {
  if (g.nx == 0 || g.ny == 0 || !(g.cell > 0) || s == 0) throw std::invalid_argument("ecology::forest::crown_cover: a grid with cells and a positive side, and s ≥ 1");
  const std::size_t sub = s * s;
  std::vector<std::uint8_t> hit(g.nx * g.ny * sub, 0);
  const double h = g.cell / static_cast<double>(s);
  for (const auto& d : discs) {
    if (!(d.r > 0)) continue;
    const double r2 = d.r * d.r;
    // The subcell columns and rows whose centres can lie within the disc.
    const long gi0 = static_cast<long>(std::floor((d.x - d.r - g.x0) / h)), gi1 = static_cast<long>(std::floor((d.x + d.r - g.x0) / h));
    const long gj0 = static_cast<long>(std::floor((d.y - d.r - g.y0) / h)), gj1 = static_cast<long>(std::floor((d.y + d.r - g.y0) / h));
    const long NX = static_cast<long>(g.nx * s), NY = static_cast<long>(g.ny * s);
    for (long J = std::max(0L, gj0); J <= std::min(NY - 1, gj1); J++) {
      const double cy = g.y0 + (static_cast<double>(J) + 0.5) * h - d.y;
      for (long I = std::max(0L, gi0); I <= std::min(NX - 1, gi1); I++) {
        const double cx = g.x0 + (static_cast<double>(I) + 0.5) * h - d.x;
        if (cx * cx + cy * cy > r2) continue;
        const std::size_t ci = static_cast<std::size_t>(I) / s, cj = static_cast<std::size_t>(J) / s;
        const std::size_t si = static_cast<std::size_t>(I) % s, sj = static_cast<std::size_t>(J) % s;
        hit[g.index(ci, cj) * sub + sj * s + si] = 1;
      }
    }
  }
  std::vector<double> cover(g.nx * g.ny, 0.0);
  for (std::size_t c = 0; c < cover.size(); c++) {
    std::size_t n = 0;
    for (std::size_t k = 0; k < sub; k++) n += hit[c * sub + k];
    cover[c] = static_cast<double>(n) / static_cast<double>(sub);
  }
  return cover;
}

// The canopy that the weight (people) of each cell lives under: the weighted share below each threshold.
struct Exposure {
  double weight = 0;              // Σ w
  double mean = 0;                // Σ w cover / Σ w
  std::vector<double> thresholds, share_below;
};

inline Exposure canopy_exposure(const std::vector<double>& cover, const std::vector<double>& weight, const std::vector<double>& thresholds) {
  if (cover.size() != weight.size()) throw std::invalid_argument("ecology::forest::canopy_exposure: one weight per cell");
  Exposure e;
  e.thresholds = thresholds;
  e.share_below.assign(thresholds.size(), 0.0);
  for (std::size_t c = 0; c < cover.size(); c++) {
    if (!(weight[c] > 0)) continue;
    e.weight += weight[c];
    e.mean += weight[c] * cover[c];
    for (std::size_t k = 0; k < thresholds.size(); k++)
      if (cover[c] < thresholds[k]) e.share_below[k] += weight[c];
  }
  if (e.weight > 0) {
    e.mean /= e.weight;
    for (double& x : e.share_below) x /= e.weight;
  }
  return e;
}

}  // namespace ecology::forest
