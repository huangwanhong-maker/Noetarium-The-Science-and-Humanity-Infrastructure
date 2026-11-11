// A building's form on a lot from a few parameters, and what can be measured of it (ADR 0089).
//
// THE MASSING. A podium fills the ground envelope (rules.hpp) to a number of storeys; an upper part, if any, stands on it within the
// upper envelope, which is moved inward evenly (physis's inset, at one distance from every edge) until it covers the share asked of
// it, to a number of further storeys. Moved in evenly, it stays within the envelope whatever its shape (scaled about a centroid, an L
// would leave it); where the inset changes construction (an arm lost, physis's inset) the share kept is the least above the one asked. Its storeys are clipped to what the height limit allows, and then, while its floor area exceeds the plot
// ratio's, the upper part loses storeys and then the podium: a massing so made complies with the rules by construction (rules.hpp's
// complies, which the checks hold it to).
//
// ITS MEASURES. Floor area (each part's footprint times its floors), the footprint's share of the lot (site coverage), volume, the
// envelope's surface (walls and roofs, the ground left out), and compactness, that surface over the volume to the power 2/3 (a cube
// standing on the ground gives 5; the lower, the less surface for the volume enclosed), and its prisms for the Sun (environment's
// sun.hpp).

#pragma once

#include <architecture/environment/sun.hpp>
#include <architecture/rules/rules.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace architecture::form {

namespace cg = physis::computational_geometry;
using architecture::rules::Lot;
using architecture::rules::Massing;
using architecture::rules::Ring;
using architecture::rules::Rules;

inline cg::Point2 centroid(const Ring& r) {
  double a = 0, cx = 0, cy = 0;
  for (std::size_t i = 0; i < r.size(); i++) {
    const auto p = r[i], q = r[(i + 1) % r.size()];
    const double c = p[0] * q[1] - q[0] * p[1];
    a += c, cx += (p[0] + q[0]) * c, cy += (p[1] + q[1]) * c;
  }
  return {cx / (3 * a), cy / (3 * a)};
}

inline Ring scaled(const Ring& r, double f) {
  const auto c = centroid(r);
  Ring out;
  for (const auto& p : r) out.push_back({c[0] + f * (p[0] - c[0]), c[1] + f * (p[1] - c[1])});
  return out;
}

// The upper envelope moved inward evenly until it covers `coverage` of its area (bisection on the distance, to 10⁻⁶ of a metre); the
// envelope itself at a coverage of 1 or more, nothing at 0 or less.
inline Ring upper_part(const Ring& upper_envelope, double coverage) {
  if (upper_envelope.size() < 3 || !(coverage > 0)) return {};
  if (coverage >= 1) return upper_envelope;
  const double whole = std::fabs(cg::ring_area(upper_envelope)), want = coverage * whole;
  const std::vector<double> none(upper_envelope.size(), 0.0);
  double lo = 0, hi = std::sqrt(whole);  // an inradius is below √(A/π): moved in by √A, nothing is left
  for (int k = 0; k < 60 && hi - lo > 1e-6; k++) {
    const double d = (lo + hi) / 2;
    const auto r = cg::inset(upper_envelope, std::vector<double>(upper_envelope.size(), d));
    if (r.size() >= 3 && std::fabs(cg::ring_area(r)) >= want) lo = d; else hi = d;
  }
  return lo > 0 ? cg::inset(upper_envelope, std::vector<double>(upper_envelope.size(), lo)) : upper_envelope;
}

// The massing on an envelope already found: `podium_storeys` on its ground, `upper_storeys` more on the upper part given (upper_part),
// storeys clipped to the height limit and taken off, the upper part's first, while the floor exceeds the plot ratio on `lot_area`.
inline Massing massing_within(const architecture::rules::Envelope& e, const Ring& upper, double lot_area, const Rules& r, int podium_storeys, int upper_storeys) {
  Massing m;
  const int cap = std::isfinite(r.max_height) ? static_cast<int>(std::floor(r.max_height / r.storey + 1e-9)) : 1000000;
  podium_storeys = std::clamp(podium_storeys, 0, cap);
  m.podium = e.ground;
  m.podium_height = podium_storeys * r.storey;
  upper_storeys = std::clamp(upper_storeys, 0, cap - podium_storeys);
  if (upper_storeys > 0 && upper.size() >= 3) {
    m.upper = upper;
    m.upper_height = m.podium_height + upper_storeys * r.storey;
  }
  const double most = r.plot_ratio * lot_area;
  while (architecture::rules::floor_area(m, r.storey) > most + 1e-6) {
    if (!m.upper.empty()) {
      m.upper_height -= r.storey;
      if (m.upper_height <= m.podium_height + 1e-9) m.upper.clear(), m.upper_height = 0;
    } else if (m.podium_height > 0) {
      m.podium_height = std::max(0.0, m.podium_height - r.storey);
    } else break;
  }
  return m;
}

// The massing of `podium_storeys` on the ground envelope and `upper_storeys` more on `coverage` of the upper envelope.
inline Massing massing(const Lot& lot, const Rules& r, int podium_storeys, int upper_storeys, double coverage) {
  const auto e = architecture::rules::envelope(lot, r);
  return massing_within(e, upper_part(e.upper, coverage), lot.area(), r, podium_storeys, upper_storeys);
}

struct Measures {
  double floor_area = 0, site_coverage = 0, volume = 0, surface = 0, compactness = 0, height = 0;
};

inline double perimeter(const Ring& r) {
  double p = 0;
  for (std::size_t i = 0; i < r.size(); i++) p += std::hypot(r[(i + 1) % r.size()][0] - r[i][0], r[(i + 1) % r.size()][1] - r[i][1]);
  return p;
}

inline Measures measures(const Massing& m, const Lot& lot, double storey) {
  Measures out;
  const double ap = std::fabs(cg::ring_area(m.podium)), au = m.upper.empty() ? 0.0 : std::fabs(cg::ring_area(m.upper));
  const double hu = m.upper.empty() ? 0.0 : m.upper_height - m.podium_height;
  out.floor_area = architecture::rules::floor_area(m, storey);
  out.site_coverage = ap / lot.area();
  out.volume = ap * m.podium_height + au * hu;
  // Walls of both parts, the podium's roof where the upper part does not stand, and the upper roof.
  out.surface = perimeter(m.podium) * m.podium_height + (m.upper.empty() ? 0.0 : perimeter(m.upper) * hu) + ap;
  out.height = m.upper.empty() ? m.podium_height : m.upper_height;
  out.compactness = out.volume > 0 ? out.surface / std::pow(out.volume, 2.0 / 3.0) : 0.0;
  return out;
}

// The massing's prisms for the Sun: the podium, and the upper part standing from the ground to its top (exact for the shadows it
// casts beyond the podium's footprint, which is where receivers are).
inline std::vector<architecture::environment::Prism> prisms(const Massing& m, int podium_variable = -1, int upper_variable = -1) {
  std::vector<architecture::environment::Prism> out{{m.podium, m.podium_height, podium_variable}};
  if (!m.upper.empty()) out.push_back({m.upper, m.upper_height, upper_variable});
  return out;
}

}  // namespace architecture::form
