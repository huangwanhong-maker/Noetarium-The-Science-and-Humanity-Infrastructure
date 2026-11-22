// Open space: the share of a place that is open space for public use, and who lives within reach of it, SDG 11.7.1 read under declared
// readings of its open terms (theory/openspace.md; ADR 0154).
//
// SDG 11.7.1 (UN-Habitat's metadata of 2025-04-23) is the share of a city's built-up area that is open space for public use, free of
// charge, streets counted apart, with the share of people within 400 m walking distance of it along the streets. Its open terms are a
// reading's: which land counts (parks and plazas; sports grounds; every green and open land, gardens and cemeteries among them), how
// large a space must be (WHO Europe's rule of thumb asks 0.5–1 ha within 300 m in a straight line), the distance, along the network or in
// a straight line, and the area the share is of. Each reading gives its own shares; none is the indicator's value until one is chosen.
//
// Spaces are polygons with holes in metres of a local frame. Their union's area, since land cover lies beneath land use and polygons
// overlap, is counted on a raster of declared cell: a cell counts when its centre lies in a space (even–odd within each polygon), so that
// the error is of the order of the perimeter times the cell. A place lies within reach when it is inside a counted space, or when its
// distance to the nearest point of one's boundary (sampled every `spacing` metres, the walking network densified as transit's) is within
// the reading's distance.

#pragma once

#include <urban/transit/transit.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace urban::openspace {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::openspace: " + what);
}
inline double ring_area(const std::vector<double>& xy) {
  double a = 0;
  const std::size_t n = xy.size() / 2;
  for (std::size_t i = 0; i < n; i++) {
    const std::size_t j = (i + 1) % n;
    a += xy[2 * i] * xy[2 * j + 1] - xy[2 * j] * xy[2 * i + 1];
  }
  return 0.5 * std::fabs(a);
}
inline bool in_ring(const std::vector<double>& xy, double x, double y) {
  bool in = false;
  const std::size_t n = xy.size() / 2;
  for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
    const double xi = xy[2 * i], yi = xy[2 * i + 1], xj = xy[2 * j], yj = xy[2 * j + 1];
    if ((yi > y) != (yj > y) && x < (xj - xi) * (y - yi) / (yj - yi) + xi) in = !in;
  }
  return in;
}
}  // namespace detail

struct Space {
  std::string kind;                         // what the caller's reading of the land made of it: "park", "plaza", "sports", "green", …
  std::vector<double> ring;                 // x0, y0, x1, y1, …
  std::vector<std::vector<double>> holes;
  void check() const {
    detail::require(ring.size() >= 6 && ring.size() % 2 == 0, "a space's ring has at least three points");
    for (const auto& h : holes) detail::require(h.size() >= 6 && h.size() % 2 == 0, "a hole has at least three points");
  }
  double area() const {
    double a = detail::ring_area(ring);
    for (const auto& h : holes) a -= detail::ring_area(h);
    return a;
  }
  bool contains(double x, double y) const {
    if (!detail::in_ring(ring, x, y)) return false;
    for (const auto& h : holes)
      if (detail::in_ring(h, x, y)) return false;
    return true;
  }
};

struct Box {
  double x0 = 0, y0 = 0, x1 = 0, y1 = 0;
  double area() const { return (x1 - x0) * (y1 - y0); }
  bool contains(double x, double y) const { return x >= x0 && x < x1 && y >= y0 && y < y1; }
};

// The area of the union of spaces on a raster of `cell` metres: cells whose centres lie in any of them (and, given a box, in it).
inline double union_area(const std::vector<const Space*>& spaces, double cell = 5, const Box* within = nullptr) {
  detail::require(cell > 0, "a cell is positive");
  if (spaces.empty()) return 0;
  double x0 = std::numeric_limits<double>::infinity(), y0 = x0, x1 = -x0, y1 = -x0;
  for (const auto* s : spaces)
    for (std::size_t k = 0; k + 1 < s->ring.size(); k += 2) x0 = std::min(x0, s->ring[k]), x1 = std::max(x1, s->ring[k]), y0 = std::min(y0, s->ring[k + 1]), y1 = std::max(y1, s->ring[k + 1]);
  const auto nx = static_cast<std::size_t>(std::ceil((x1 - x0) / cell)), ny = static_cast<std::size_t>(std::ceil((y1 - y0) / cell));
  std::vector<char> hit(nx * ny, 0);
  for (const auto* s : spaces) {
    double sx0 = std::numeric_limits<double>::infinity(), sy0 = sx0, sx1 = -sx0, sy1 = -sx0;
    for (std::size_t k = 0; k + 1 < s->ring.size(); k += 2) sx0 = std::min(sx0, s->ring[k]), sx1 = std::max(sx1, s->ring[k]), sy0 = std::min(sy0, s->ring[k + 1]), sy1 = std::max(sy1, s->ring[k + 1]);
    const auto i0 = static_cast<std::size_t>(std::max(0.0, std::floor((sx0 - x0) / cell))), i1 = std::min(nx, static_cast<std::size_t>(std::ceil((sx1 - x0) / cell)));
    const auto j0 = static_cast<std::size_t>(std::max(0.0, std::floor((sy0 - y0) / cell))), j1 = std::min(ny, static_cast<std::size_t>(std::ceil((sy1 - y0) / cell)));
    for (std::size_t j = j0; j < j1; j++)
      for (std::size_t i = i0; i < i1; i++)
        if (!hit[j * nx + i]) {
          const double cx = x0 + (i + 0.5) * cell, cy = y0 + (j + 0.5) * cell;
          if ((!within || within->contains(cx, cy)) && s->contains(cx, cy)) hit[j * nx + i] = 1;
        }
  }
  double n = 0;
  for (const char h : hit) n += h;
  return n * cell * cell;
}

// Points along a space's outer ring, no two more than `spacing` metres apart along it.
inline std::vector<std::pair<double, double>> edge_points(const Space& s, double spacing) {
  std::vector<std::pair<double, double>> out;
  const std::size_t n = s.ring.size() / 2;
  for (std::size_t i = 0; i < n; i++) {
    const std::size_t j = (i + 1) % n;
    const double x0 = s.ring[2 * i], y0 = s.ring[2 * i + 1], x1 = s.ring[2 * j], y1 = s.ring[2 * j + 1];
    const int parts = std::max(1, static_cast<int>(std::ceil(std::hypot(x1 - x0, y1 - y0) / spacing)));
    for (int k = 0; k < parts; k++) out.push_back({x0 + (x1 - x0) * k / parts, y0 + (y1 - y0) * k / parts});
  }
  return out;
}

struct Reading {
  std::string name;
  std::set<std::string> kinds;    // the kinds of land that count
  double least_area = 0;          // m²: a space smaller does not count
  double metres = 400;
  bool along_network = true;
  std::string source;
};

struct Result {
  std::string reading;
  double people = 0, within = 0, share = 0;
  std::size_t spaces = 0;
  double area = 0;                // m², of the counted spaces' union
  double area_share = 0;          // of the analysis area
  std::vector<double> distance;   // m from each place to the nearest counted space (0 inside one)
  std::vector<char> served;
};

// The analysis area is `analysis_area`, or, given a box, the box: spaces then count only within it.
inline Result reach(const access::Network& walk, const std::vector<transit::Place>& places, const std::vector<Space>& spaces, const Reading& r, double analysis_area,
                    double spacing = 25, double cell = 5, const Box* within = nullptr) {
  if (within) analysis_area = within->area();
  detail::require(r.metres > 0 && r.least_area >= 0 && analysis_area > 0 && spacing > 0, "a reading's distance, least area, the analysis area and the spacing are positive");
  Result out;
  out.reading = r.name;
  std::vector<const Space*> counted;
  for (const auto& s : spaces) {
    s.check();
    if (r.kinds.count(s.kind) && s.area() >= r.least_area) counted.push_back(&s);
  }
  out.spaces = counted.size();
  out.area = union_area(counted, cell, within);
  out.area_share = out.area / analysis_area;
  std::vector<std::pair<double, double>> supply, demand;
  for (const auto* s : counted)
    for (const auto& p : edge_points(*s, spacing)) supply.push_back(p);
  for (const auto& p : places) {
    detail::require(p.people >= 0, "a place's people are at least zero");
    demand.push_back({p.x, p.y});
  }
  out.distance.assign(places.size(), transit::never);
  if (!supply.empty() && !demand.empty()) {
    if (r.along_network) out.distance = access::nearest_supply(walk, demand, supply, 1.0);
    else
      for (std::size_t i = 0; i < demand.size(); i++)
        for (const auto& [x, y] : supply) out.distance[i] = std::min(out.distance[i], std::hypot(demand[i].first - x, demand[i].second - y));
  }
  for (std::size_t i = 0; i < places.size(); i++) {
    for (const auto* s : counted)
      if (s->contains(places[i].x, places[i].y)) {
        out.distance[i] = 0;
        break;
      }
    const bool ok = out.distance[i] <= r.metres;
    out.served.push_back(ok);
    out.people += places[i].people;
    if (ok) out.within += places[i].people;
  }
  out.share = out.people > 0 ? out.within / out.people : 0;
  return out;
}

}  // namespace urban::openspace
