// The rules that bound a building's form, the envelope they allow on a lot, and whether a massing lies within them (ADR 0089).
//
// THE LOT. A footprint ring in metres and, for each of its edges, whether it faces a street. THE RULES, as a planning scheme states them
// (declared by the caller; none here is any city's):
// - a height limit;
// - a plot ratio, the floor area over the lot's area;
// - setbacks: from the street edges and from the others, at the ground;
// - a street wall: the podium may rise on the street line to a height, above which the building steps back from the street edges by
//   an upper setback;
// - the storey height that turns heights into floors.
//
// THE ENVELOPE. The ground footprint allowed is the lot moved inward by its setbacks (physis's inset); above the street wall, the
// footprint allowed is moved further from the street edges by the upper setback. A massing (form.hpp) is a podium and an upper part,
// each a ring and a height; it complies when
// - each part lies within its envelope, its ring inside the allowed ring (every vertex, within a tolerance);
// - the podium is no higher than the street wall unless it is set back as the upper part must be;
// - its height is within the limit;
// - its floor area is within the plot ratio.
// Each failure is named.
//
// A PASSAGE. A strip of a width centred on a line across a lot (a lane, an arcade open to the sky) cuts it into the parts either side
// (physis's clip_left); their edges along the strip face it, as a street. A lot with reflex vertices may come apart into pieces
// joined along the cut, which are kept as one ring (its area right, the joins degenerate edges on the cut).

#pragma once

#include <physis/computational_geometry/planar.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace architecture::rules {

namespace cg = physis::computational_geometry;
using Ring = std::vector<cg::Point2>;

struct Lot {
  Ring ring;
  std::vector<char> street;   // per edge (k from vertex k to k + 1): whether it faces a street
  double area() const { return std::fabs(cg::ring_area(ring)); }
};

struct Rules {
  double max_height = std::numeric_limits<double>::infinity();
  double plot_ratio = std::numeric_limits<double>::infinity();
  double street_setback = 0, side_setback = 0;
  double street_wall = std::numeric_limits<double>::infinity();
  double upper_setback = 0;
  double storey = 3.2;
};

struct Envelope {
  Ring ground, upper;          // the footprints allowed below and above the street wall
  double max_height = 0, max_floor_area = 0;
};

struct Massing {
  Ring podium;
  double podium_height = 0;
  Ring upper;                  // empty for none
  double upper_height = 0;     // the top of the upper part, measured from the ground
};

struct Compliance {
  bool ok = true;
  std::vector<std::string> reasons;
  double floor_area = 0, plot_ratio = 0, height = 0;
};

inline Envelope envelope(const Lot& lot, const Rules& r) {
  const std::size_t n = lot.ring.size();
  std::vector<double> d0(n), d1(n);
  for (std::size_t k = 0; k < n; k++) {
    const bool st = k < lot.street.size() && lot.street[k];
    d0[k] = st ? r.street_setback : r.side_setback;
    d1[k] = st ? r.street_setback + r.upper_setback : r.side_setback;
  }
  Envelope e;
  e.ground = cg::inset(lot.ring, d0);
  e.upper = cg::inset(lot.ring, d1);
  e.max_height = r.max_height;
  e.max_floor_area = r.plot_ratio * lot.area();
  return e;
}

// The floors of a part of a given height: floor(h / storey + 1e-9).
inline double floors(double height, double storey) { return std::floor(height / storey + 1e-9); }

inline double floor_area(const Massing& m, double storey) {
  const double fp = std::fabs(cg::ring_area(m.podium)), fu = m.upper.empty() ? 0.0 : std::fabs(cg::ring_area(m.upper));
  if (m.upper.empty()) return fp * floors(m.podium_height, storey);
  return fp * floors(m.podium_height, storey) + fu * floors(std::max(0.0, m.upper_height - m.podium_height), storey);
}

namespace detail {
inline bool within(const Ring& inner, const Ring& outer, double tolerance) {
  if (outer.size() < 3) return false;
  for (const auto& p : inner) {
    if (cg::inside_ring(outer, p)) continue;
    // On the boundary within the tolerance counts as within.
    double best = std::numeric_limits<double>::infinity();
    for (std::size_t k = 0; k < outer.size(); k++) {
      const auto a = outer[k], b = outer[(k + 1) % outer.size()];
      const double ex = b[0] - a[0], ey = b[1] - a[1], L2 = ex * ex + ey * ey;
      const double t = L2 > 0 ? std::clamp(((p[0] - a[0]) * ex + (p[1] - a[1]) * ey) / L2, 0.0, 1.0) : 0.0;
      best = std::min(best, std::hypot(p[0] - a[0] - t * ex, p[1] - a[1] - t * ey));
    }
    if (best > tolerance) return false;
  }
  return true;
}
}  // namespace detail

inline Compliance complies(const Massing& m, const Lot& lot, const Rules& r, double tolerance = 0.01) {
  const auto e = envelope(lot, r);
  Compliance c;
  c.height = m.upper.empty() ? m.podium_height : std::max(m.podium_height, m.upper_height);
  c.floor_area = floor_area(m, r.storey);
  c.plot_ratio = c.floor_area / lot.area();
  const auto fail = [&](std::string why) { c.ok = false, c.reasons.push_back(std::move(why)); };
  if (!detail::within(m.podium, e.ground, tolerance)) fail("the podium outside the setbacks");
  if (m.podium_height > r.street_wall + 1e-9 && !detail::within(m.podium, e.upper, tolerance)) fail("the podium above the street wall on the street line");
  if (!m.upper.empty() && !detail::within(m.upper, e.upper, tolerance)) fail("the upper part not set back from the street");
  if (c.height > r.max_height + 1e-9) fail("above the height limit");
  if (c.floor_area > e.max_floor_area + 1e-6) fail("beyond the plot ratio");
  return c;
}

// The parts of a lot either side of a passage of `width` centred on the line through a and b, each of more than `least` m².
inline std::vector<Lot> split(const Lot& lot, cg::Point2 a, cg::Point2 b, double width, double least = 1.0) {
  const double L = std::hypot(b[0] - a[0], b[1] - a[1]);
  if (!(L > 0)) return {lot};
  const double nx = -(b[1] - a[1]) / L * width / 2, ny = (b[0] - a[0]) / L * width / 2;
  const auto on = [](cg::Point2 p, cg::Point2 c, cg::Point2 d) {
    return std::fabs((d[0] - c[0]) * (p[1] - c[1]) - (d[1] - c[1]) * (p[0] - c[0])) <= 1e-7 * (1 + std::hypot(d[0] - c[0], d[1] - c[1]));
  };
  std::vector<Lot> out;
  for (int side = 0; side < 2; side++) {
    const cg::Point2 c = side == 0 ? cg::Point2{a[0] + nx, a[1] + ny} : cg::Point2{b[0] - nx, b[1] - ny};
    const cg::Point2 d = side == 0 ? cg::Point2{b[0] + nx, b[1] + ny} : cg::Point2{a[0] - nx, a[1] - ny};
    Lot part;
    part.ring = cg::clip_left(lot.ring, c, d);
    if (part.ring.size() < 3 || part.area() <= least) continue;
    const std::size_t n = part.ring.size(), m = lot.ring.size();
    part.street.assign(n, 0);
    for (std::size_t k = 0; k < n; k++) {
      const auto p = part.ring[k], q = part.ring[(k + 1) % n];
      if (on(p, c, d) && on(q, c, d)) { part.street[k] = 1; continue; }
      // Otherwise a piece of an edge of the lot: the one its midpoint lies on.
      const cg::Point2 mid{(p[0] + q[0]) / 2, (p[1] + q[1]) / 2};
      std::size_t best = 0;
      double gap = std::numeric_limits<double>::infinity();
      for (std::size_t j = 0; j < m; j++) {
        const double g = cg::segment_distance(mid, lot.ring[j], lot.ring[(j + 1) % m]);
        if (g < gap) gap = g, best = j;
      }
      part.street[k] = best < lot.street.size() ? lot.street[best] : 0;
    }
    out.push_back(std::move(part));
  }
  return out;
}

}  // namespace architecture::rules
