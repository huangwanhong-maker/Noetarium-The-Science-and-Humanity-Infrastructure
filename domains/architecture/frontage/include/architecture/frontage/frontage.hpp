// The frontage: where a building meets its street (ADR 0089). Its physical part only; the symbolic part (what a frontage says) is
// planned apart (README).
//
// THE STREET WALL. Of the lot's street edges, the share of their length along which the podium stands on the street line: a sample of
// the line set back by the street setback is walled when the point a tolerance further in lies inside the podium (so a podium a little
// short of the line still counts, and its corners count no further than they reach). A continuous wall gives 1, a building set in its
// lot 0. Its height is the
// podium's where it stands there. A frontage is active where its ground floor holds uses that face the street (shops, cafés, lobbies),
// which the caller declares as a share; the active length is the street wall's length times that share.

#pragma once

#include <architecture/rules/rules.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace architecture::frontage {

namespace cg = physis::computational_geometry;

struct Frontage {
  double street_length = 0;   // the lot's street edges
  double wall_length = 0;     // of them, where the podium stands on the street line
  double share = 0;           // wall / street
  double wall_height = 0;     // the podium's, where it stands there
  double active_length = 0;   // the wall's length times the active share declared
};

inline Frontage frontage(const architecture::rules::Massing& m, const architecture::rules::Lot& lot, const architecture::rules::Rules& r, double active_share = 0,
                         double tolerance = 0.5, double step = 0.5) {
  Frontage f;
  auto ring = lot.ring;
  auto street = lot.street;
  if (cg::ring_area(ring) < 0) {
    std::reverse(ring.begin(), ring.end());
    const std::size_t n = street.size();
    std::vector<char> s2(n);
    for (std::size_t k = 0; k < n; k++) s2[(2 * n - 2 - k) % n] = street[k];
    street = s2;
  }
  const std::size_t n = ring.size();
  for (std::size_t k = 0; k < n; k++) {
    if (k >= street.size() || !street[k]) continue;
    const auto a = ring[k], b = ring[(k + 1) % n];
    const double L = std::hypot(b[0] - a[0], b[1] - a[1]);
    if (L <= 0) continue;
    const double nx = -(b[1] - a[1]) / L, ny = (b[0] - a[0]) / L;  // inward
    f.street_length += L;
    // Sample the line set back by the street setback, at the middles of steps of at most `step`.
    const int steps = std::max(1, static_cast<int>(std::ceil(L / step)));
    const double seg = L / steps, depth = r.street_setback + tolerance;
    for (int i = 0; i < steps; i++) {
      const double s = (i + 0.5) * seg;
      const cg::Point2 p{a[0] + (b[0] - a[0]) * s / L + depth * nx, a[1] + (b[1] - a[1]) * s / L + depth * ny};
      if (m.podium_height > 0 && cg::inside_ring(m.podium, p)) f.wall_length += seg;
    }
  }
  f.share = f.street_length > 0 ? f.wall_length / f.street_length : 0.0;
  f.wall_height = f.wall_length > 0 ? m.podium_height : 0.0;
  f.active_length = f.wall_length * active_share;
  return f;
}

}  // namespace architecture::frontage
