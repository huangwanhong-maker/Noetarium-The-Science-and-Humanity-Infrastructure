// Disruption: what a hazard's intensity does to the links of a city's roads, and the scenarios of its return periods
// (theory/disaster-risk.md §2).
//
// A LINK UNDER A HAZARD. A map of a hazard's intensity (a flood's or a tsunami's depth) on a grid of longitude and latitude (urban
// science's exposure::Grid) is read along each link every few metres, and the greatest depth found stands for the link: a road is
// as passable as its deepest point. A depth–disruption curve gives the share of its speed the link keeps.
//
// CURVES. For vehicles, Pregnolato et al. (2017, Transportation Research Part D 55, 67, eq. 4) fitted the speed of a car through
// standing water of depth w (mm) as v(w) = 0.0009 w² − 0.5529 w + 86.9448 km/h, and set 300 mm as the depth at which a car stops
// (their Table 2); the quadratic does not reach zero (its least, about 2 km/h, is near 307 mm), so the curve here is v(w)/v(0) below
// 300 mm and 0 from it. For people on foot no curve of that standing is held here: `walking_declared` falls linearly from 1 at no
// depth to 0 at 0.5 m, a declaration a work may replace, and says so. Any curve is a table of depths and shares, read linearly.
//
// THE FRAME. Roads come in a local frame (metres east and north of a centre) and maps in longitude and latitude; `Frame` carries one
// to the other with WGS 84's radii of curvature at the centre, the meridian's M and the prime vertical's N (a metre north is 1/M
// radians of latitude, a metre east 1/(N cos φ₀) of longitude). Against the exact local frame of the workbench (east, north, up on
// the ellipsoid) it departs by terms of order d²/R: under a metre within 10 km of the centre.
//
// SCENARIOS. The maps of a hazard at return periods T_1 < … < T_m give states; with the undisturbed state they are weighted by risk
// management's curve_weights (nothing below the first map unless asked), which integrate any loss over the annual exceedance probability by trapezoids.

#pragma once

#include <risk_management/criticality/criticality.hpp>
#include <urban/exposure/exposure.hpp>
#include <urban_risk_management/lifelines/lifelines.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace disaster_risk::disruption {

namespace ll = urban_risk_management::lifelines;
namespace rc = risk_management::criticality;
namespace ue = urban::exposure;

struct Curve {
  std::string name;
  std::vector<double> depth;  // m, ascending from 0
  std::vector<double> share;  // the share of speed kept, in [0, 1]
  std::string source;

  double operator()(double d) const {
    if (d <= depth.front()) return share.front();
    if (d >= depth.back()) return share.back();
    const auto it = std::upper_bound(depth.begin(), depth.end(), d);
    const std::size_t k = static_cast<std::size_t>(it - depth.begin());
    const double t = (d - depth[k - 1]) / (depth[k] - depth[k - 1]);
    return share[k - 1] + t * (share[k] - share[k - 1]);
  }
  void check() const {
    if (depth.size() != share.size() || depth.size() < 2) throw std::invalid_argument("disaster_risk::disruption: a curve has a share at each of at least two depths");
    for (std::size_t k = 0; k < depth.size(); k++) {
      if (!(share[k] >= 0 && share[k] <= 1)) throw std::invalid_argument("disaster_risk::disruption: shares lie in [0, 1]");
      if (k > 0 && !(depth[k] > depth[k - 1])) throw std::invalid_argument("disaster_risk::disruption: depths ascend");
    }
  }
};

// Cars in standing water: Pregnolato et al. 2017, eq. 4, stopped from 300 mm (Table 2), tabulated every 10 mm.
inline Curve pregnolato_2017() {
  Curve c{"vehicles (Pregnolato et al. 2017)", {}, {}, "Pregnolato, Ford, Wilkinson and Dawson (2017), Transportation Research Part D 55, 67–81, eq. 4 and Table 2"};
  const auto v = [](double w) { return 0.0009 * w * w - 0.5529 * w + 86.9448; };
  for (int w = 0; w < 300; w += 10) {
    c.depth.push_back(w / 1000.0);
    c.share.push_back(v(w) / v(0));
  }
  c.depth.push_back(0.29999);
  c.share.push_back(v(299.99) / v(0));
  c.depth.push_back(0.3);
  c.share.push_back(0);
  c.depth.push_back(100);
  c.share.push_back(0);
  return c;
}

// People on foot: a declaration, not a standard (1 at no depth, 0 from 0.5 m).
inline Curve walking_declared() {
  return {"on foot (declared)", {0, 0.5, 100}, {1, 0, 0}, "a declaration of this domain, to be replaced by a work's own"};
}

// A local frame: metres east and north of a centre, with WGS 84's radii of curvature there.
struct Frame {
  double lat0 = 0, lon0 = 0;  // degrees
  static constexpr double a = 6378137.0, f = 1 / 298.257223563, degree = 3.14159265358979323846 / 180;
  double meridian() const {  // M at the centre, m
    const double e2 = f * (2 - f), s = std::sin(lat0 * degree);
    return a * (1 - e2) / std::pow(1 - e2 * s * s, 1.5);
  }
  double prime() const {  // N at the centre, m
    const double e2 = f * (2 - f), s = std::sin(lat0 * degree);
    return a / std::sqrt(1 - e2 * s * s);
  }
  double east(double lon) const { return (lon - lon0) * degree * prime() * std::cos(lat0 * degree); }
  double north(double lat) const { return (lat - lat0) * degree * meridian(); }
  double lon(double x) const { return lon0 + x / (degree * prime() * std::cos(lat0 * degree)); }
  double lat(double y) const { return lat0 + y / (degree * meridian()); }
};

struct LinkDepth {
  std::vector<double> deepest;  // per link, m (0 where dry or outside the map)
  std::vector<char> mapped;     // per link: some sample lay on the map
};

// The greatest depth along each link, read every `step` metres of its pieces.
inline LinkDepth depths(const ll::Graph& g, const ue::Grid& grid, const Frame& f, double step = 5) {
  grid.check();
  if (!(step > 0)) throw std::invalid_argument("disaster_risk::disruption::depths: a step > 0");
  LinkDepth out;
  out.deepest.assign(g.links.size(), 0);
  out.mapped.assign(g.links.size(), 0);
  for (std::size_t l = 0; l < g.links.size(); l++) {
    const auto& P = g.links[l].points;
    for (std::size_t k = 0; k + 1 < P.size(); k++) {
      const double x0 = g.x[P[k]], y0 = g.y[P[k]], x1 = g.x[P[k + 1]], y1 = g.y[P[k + 1]];
      const int n = std::max(1, static_cast<int>(std::ceil(std::hypot(x1 - x0, y1 - y0) / step)));
      for (int i = 0; i <= n; i++) {
        const double t = static_cast<double>(i) / n;
        const double d = grid.at(f.lat(y0 + t * (y1 - y0)), f.lon(x0 + t * (x1 - x0)));
        if (d >= 0) {
          out.mapped[l] = 1;
          out.deepest[l] = std::max(out.deepest[l], d);
        }
      }
    }
  }
  return out;
}

// The state of the links: the share of speed each keeps at its deepest point.
inline ll::State state(const LinkDepth& d, const Curve& curve) {
  curve.check();
  ll::State phi(d.deepest.size());
  for (std::size_t l = 0; l < phi.size(); l++) phi[l] = curve(d.deepest[l]);
  return phi;
}

// The scenarios of maps at ascending return periods, with the undisturbed state first, weighted to integrate over the curve.
inline std::vector<rc::Scenario> scenarios(const std::vector<double>& return_periods, const std::vector<ll::State>& states, bool linear_below = false) {
  if (return_periods.size() != states.size()) throw std::invalid_argument("disaster_risk::disruption::scenarios: a state for each return period");
  const auto w = rc::curve_weights(return_periods, linear_below);
  std::vector<rc::Scenario> out{{"undisturbed", w[0], {}}};
  for (std::size_t k = 0; k < states.size(); k++) out.push_back({std::to_string(static_cast<long long>(std::llround(return_periods[k]))) + "-year", w[k + 1], states[k]});
  return out;
}

}  // namespace disaster_risk::disruption
