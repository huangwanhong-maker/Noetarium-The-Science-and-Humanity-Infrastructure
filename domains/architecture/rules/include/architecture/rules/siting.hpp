// The siting of a building on its lot, rule by rule (ADR 0095): what a planning scheme states of a dwelling's height, its setbacks,
// the walls it may stand on a boundary, the land it may cover and the land it must leave open.
//
// THE BUILDING is given storey by storey as LEVELS: the outer walls of a storey as segments on their centrelines, their thickness, the
// height of the storey's top above the ground, and the floor it covers (to the centrelines). THE LOT is rules.hpp's: a ring and, for
// each edge, whether it faces a street.
//
// THE SCHEME states limits (finding.hpp); a limit not stated is not evaluated. Its forms are those planning schemes use:
// - a height and a number of storeys, at most;
// - a setback from the street edges, at least;
// - a setback from the other edges that grows with a wall's height, piecewise linearly: s(h) = s₀ + Σ_i m_i · clamp(h − a_i, 0, b_i − a_i).
//   Victoria's one-dwelling standard is s₀ = 1 m with (a, b, m) = (3.6, 6.9, 0.3) and (6.9, ∞, 1): s(6.9) = 1.99 m, s(9) = 4.09 m;
// - walls on a boundary: a wall within a tolerance of an edge that is not a street stands on it, and is exempt from the setback. On
//   each such edge of length L the walls standing on it may total at most b + q (L − b) (the whole edge when L ≤ b), and the storey
//   that holds them may rise at most to a height;
// - the share of the lot the building covers, at most; the land left uncovered, at least a share of the lot and at least an area; and
//   a garden share that depends on the lot's size, a table of (lot area above, share).
//
// HOW DISTANCES ARE MEASURED. A wall's face stands half its thickness outside its centreline. The distance of a storey from an edge
// is the least distance of its outer walls' ends to that edge (a segment, not its line), less half the thickness; ends within the
// tolerance of the edge belong to walls on the boundary and are left out. The least distance between a wall and an edge is attained
// at an end of the wall unless an end of the edge is nearer, a case left out here (a lot's corner pointing into a wall).
//
// WHAT IS COVERED. The storey of the greatest floor, to the outer faces of its walls: its floor to the centrelines plus half the
// thickness along its outer walls' length, the corners' squares left out (4 (t/2)² for a rectangle). Exact to that when the storeys
// nest. Uncovered land is counted as open space and as garden; paving is not drawn, so permeability is not evaluated.

#pragma once

#include <architecture/rules/finding.hpp>
#include <architecture/rules/rules.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

namespace architecture::rules {

struct Segment {
  cg::Point2 a{}, b{};
  double length() const { return std::hypot(b[0] - a[0], b[1] - a[1]); }
};

struct Level {
  std::vector<Segment> walls;   // the storey's outer walls, on their centrelines
  double thickness = 0.25;      // of those walls (m)
  double top = 3;               // the height of the storey's top above the ground (m)
  double floor = 0;             // the floor it covers, to the centrelines (m²)
};

struct Step {
  double above = 0, to = std::numeric_limits<double>::infinity(), slope = 0;
};

struct GardenRow {
  double lot_above = 0, share = 0;   // lots larger than `lot_above` m² keep at least `share` as garden
};

struct Scheme {
  std::string name, version, source;
  Limit height, storeys, street_setback, side_setback, coverage, open_share, open_area, boundary_height;
  std::vector<Step> side_steps;
  double boundary_base = std::numeric_limits<double>::quiet_NaN(), boundary_share = 0;   // b and q of b + q (L − b)
  std::string boundary_clause;
  double boundary_tolerance = 0.2;
  std::vector<GardenRow> garden;
  std::string garden_clause;
};

// The setback from an edge that is not a street, required of a wall whose top is at `height`.
inline double side_setback(const Scheme& s, double height) {
  double v = std::isnan(s.side_setback.least) ? 0.0 : s.side_setback.least;
  for (const auto& st : s.side_steps) v += st.slope * std::clamp(height - st.above, 0.0, st.to - st.above);
  return v;
}

// The length of walls an edge of length L may carry.
inline double boundary_allowance(const Scheme& s, double L) {
  if (std::isnan(s.boundary_base)) return std::numeric_limits<double>::infinity();
  return L <= s.boundary_base ? L : s.boundary_base + s.boundary_share * (L - s.boundary_base);
}

// The share of garden a lot of an area must keep: the row of the greatest `lot_above` below the area; 0 when none applies.
inline double garden_share(const Scheme& s, double lot_area) {
  double share = 0, best = -1;
  for (const auto& g : s.garden)
    if (lot_area > g.lot_above && g.lot_above > best) best = g.lot_above, share = g.share;
  return share;
}

struct Siting {
  std::vector<Finding> findings;
  double covered = 0, open = 0, height = 0;
  std::vector<double> on_boundary;   // per lot edge: the length of walls standing on it
};

inline Siting check(const Lot& lot, const std::vector<Level>& levels, const Scheme& s) {
  Siting out;
  const std::size_t n = lot.ring.size();
  out.on_boundary.assign(n, 0.0);
  if (n < 3 || levels.empty()) return out;
  const double A = lot.area();
  const auto is_street = [&](std::size_t k) { return k < lot.street.size() && lot.street[k]; };
  const auto edge_distance = [&](cg::Point2 p, std::size_t k) { return cg::segment_distance(p, lot.ring[k], lot.ring[(k + 1) % n]); };
  const auto add = [&](Finding f) { out.findings.push_back(std::move(f)); };

  // Within the lot: every end of an outer wall inside it, or on its boundary within the tolerance.
  {
    std::size_t outsiders = 0;
    for (const auto& L : levels)
      for (const auto& w : L.walls)
        for (const cg::Point2 p : {w.a, w.b}) {
          if (cg::inside_ring(lot.ring, p)) continue;
          double best = std::numeric_limits<double>::infinity();
          for (std::size_t k = 0; k < n; k++) best = std::min(best, edge_distance(p, k));
          if (best > s.boundary_tolerance) outsiders++;
        }
    add(found("lot", "Within the lot", "the building", Limit::at_most(0), static_cast<double>(outsiders), "count",
              outsiders ? "ends of outer walls beyond the lot's boundary" : ""));
  }

  for (const auto& L : levels) out.height = std::max(out.height, L.top);
  if (s.height.stated()) add(found("height", "Building height", "the building", s.height, out.height, "m"));
  if (s.storeys.stated()) add(found("storeys", "Storeys", "the building", s.storeys, static_cast<double>(levels.size()), "count"));

  // The street setback: the least distance of any storey's face from any street edge.
  if (s.street_setback.stated()) {
    double least = std::numeric_limits<double>::infinity();
    for (const auto& L : levels)
      for (const auto& w : L.walls)
        for (const cg::Point2 p : {w.a, w.b})
          for (std::size_t k = 0; k < n; k++)
            if (is_street(k)) least = std::min(least, edge_distance(p, k) - L.thickness / 2);
    if (std::isfinite(least)) add(found("street-setback", "Street setback", "the building", s.street_setback, least, "m", "", 1e-6));
  }

  // The other edges, storey by storey: walls on the boundary, and the setback of the rest.
  std::vector<double> boundary_top(n, 0.0);
  for (std::size_t l = 0; l < levels.size(); l++) {
    const auto& L = levels[l];
    const double reach = s.boundary_tolerance + L.thickness / 2;
    double least = std::numeric_limits<double>::infinity();
    std::vector<double> on(n, 0.0);
    for (const auto& w : L.walls)
      for (std::size_t k = 0; k < n; k++) {
        if (is_street(k)) continue;
        const double da = edge_distance(w.a, k), db = edge_distance(w.b, k);
        if (da <= reach && db <= reach) { on[k] += w.length(); continue; }
        for (const double d : {da, db})
          if (d > reach) least = std::min(least, d - L.thickness / 2);
      }
    for (std::size_t k = 0; k < n; k++)
      if (on[k] > 0) out.on_boundary[k] = std::max(out.on_boundary[k], on[k]), boundary_top[k] = std::max(boundary_top[k], L.top);
    if (s.side_setback.stated() && std::isfinite(least)) {
      Limit need = Limit::at_least(side_setback(s, L.top), s.side_setback.clause);
      char high[32];
      std::snprintf(high, sizeof high, "%.1f", L.top);
      add(found("side-setback", "Side and rear setbacks", "storey " + std::to_string(l + 1), need, least, "m", std::string("for a wall ") + high + " m high", 1e-6));
    }
  }
  for (std::size_t k = 0; k < n; k++) {
    if (!(out.on_boundary[k] > 0)) continue;
    const double Lk = std::hypot(lot.ring[(k + 1) % n][0] - lot.ring[k][0], lot.ring[(k + 1) % n][1] - lot.ring[k][1]);
    if (!std::isnan(s.boundary_base))
      add(found("boundary-length", "Walls on a boundary: their length", "edge " + std::to_string(k + 1),
                Limit::at_most(boundary_allowance(s, Lk), s.boundary_clause), out.on_boundary[k], "m", "", 1e-6));
    if (s.boundary_height.stated())
      add(found("boundary-height", "Walls on a boundary: their height", "edge " + std::to_string(k + 1), s.boundary_height, boundary_top[k], "m"));
  }

  // What is covered: the storey of the greatest floor, to its walls' outer faces.
  {
    double best = -1;
    for (const auto& L : levels) {
      double run = 0;
      for (const auto& w : L.walls) run += w.length();
      const double c = L.floor + run * L.thickness / 2;
      if (c > best) best = c;
    }
    out.covered = std::max(0.0, best);
    out.open = A - out.covered;
    const std::string note = levels.size() > 1 ? "the storey of the greatest floor; exact when the storeys nest" : "";
    if (s.coverage.stated()) add(found("coverage", "Site coverage", "the lot", s.coverage, out.covered / A, "share", note));
    if (s.open_share.stated() || s.open_area.stated()) {
      const double need = std::max(std::isnan(s.open_share.least) ? 0.0 : s.open_share.least * A, std::isnan(s.open_area.least) ? 0.0 : s.open_area.least);
      add(found("open-space", "Open space", "the lot", Limit::at_least(need, s.open_share.stated() ? s.open_share.clause : s.open_area.clause), out.open, "m²",
                "all uncovered land is counted; its seclusion and its least width are not drawn"));
    }
    const double g = garden_share(s, A);
    if (g > 0) add(found("garden", "Garden area", "the lot", Limit::at_least(g, s.garden_clause), out.open / A, "share", "uncovered land is counted as garden; paving is not drawn"));
  }
  return out;
}

}  // namespace architecture::rules
