// What a change to the streets does to what is counted there, and to what is counted near it (ADR 0087).
//
// THE OBJECTS. A city's sensors (counters of walkers or of bicycles, any count taken at a place) are units, observed in periods. Its
// interventions are treatments that stand from a period on: a street closed, a lane built, a building opened, a station opened.
// Each has a footprint on the network of ways: a polyline, or a single point for a place. The distance that carries a spillover is
// the network's, the metres along the ways from the footprint to the sensor, and not the distance over the ground: a lane draws
// riders from the streets one could ride instead, not from those across a river.
//
// WHAT IS HERE. The network distances from every sensor to every footprint within a reach, as the contacts of peira's exposure
// mapping (contacts). Each treated sensor's own change against the sensors never reached (unit_effects), to draw on a map. The whole
// chain on a city's records in one call (evaluate), as the workbench and the studies run it. The
// estimators (direct and spillover effects, by staggered adoption or in a two-way regression, synthetic control with clean donors,
// the test of no spillovers) are peira's interference, used as they are. Nothing here names a city.
//
// DISTANCES. The network is access's, built from the ways at a speed of 1 m/s, so that its times are metres. A footprint is sampled
// every `step` metres along its length. Each sample and each sensor joins the network as access joins a place: at its nearest point
// in a part holding 1% of the network's points, with the metres between added. One search from all the samples, bounded by the
// reach, gives each sensor's distance.
//
// A SENSOR'S OWN CHANGE. For a sensor entering ring r at period g, with windows of w periods,
//   δ_i = (ȳ_i[g, g + w) − ȳ_i[g − w, g)) − (ȳ_C[g, g + w) − ȳ_C[g − w, g)),
// the change of its mean across its entry against the mean change over the same periods of the sensors C not reached by the end of
// the second window (or never), counted in both: the difference in differences of one unit, with the comparison of the staggered
// estimator. It is the group-time effect of a group of one, as noisy as one sensor is, and is drawn, not tested.

#pragma once

#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>

#include <calx/random/stream.hpp>

#include <peira/interference/interference.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace urban::interventions {

namespace pi = peira::interference;

struct Place {
  double x = 0, y = 0;  // metres, in the local frame
};

struct Footprint {
  std::vector<double> xy;   // x0, y0, x1, y1, …: a polyline, or one point
  std::int64_t begin = 0;   // the period from which it stands
};

// The points of a polyline every `step` metres (its vertices kept).
inline std::vector<std::pair<double, double>> samples(const std::vector<double>& xy, double step) {
  if (xy.size() < 2 || xy.size() % 2) throw std::invalid_argument("interventions: a footprint's points come in pairs");
  std::vector<std::pair<double, double>> out{{xy[0], xy[1]}};
  for (std::size_t i = 2; i + 1 < xy.size(); i += 2) {
    const double ax = xy[i - 2], ay = xy[i - 1], bx = xy[i], by = xy[i + 1], L = std::hypot(bx - ax, by - ay);
    const int n = std::max(1, static_cast<int>(std::ceil(L / step)));
    for (int k = 1; k <= n; k++) out.push_back({ax + (bx - ax) * k / n, ay + (by - ay) * k / n});
  }
  return out;
}

// The network distances (metres) from each place to each footprint within `reach`, as contacts for peira's exposure; `metres` is a
// network whose times are metres (its ways at 1 m/s).
inline std::vector<pi::Contact> contacts(const access::Network& metres, const std::vector<Place>& places, const std::vector<Footprint>& footprints, double reach,
                                         double step = 25) {
  if (!(reach > 0) || !(step > 0)) throw std::invalid_argument("interventions: a reach and a step, positive");
  const auto ok = access::joinable(metres);
  std::vector<access::Joined> at;
  for (const auto& p : places) at.push_back(access::join(metres, p.x, p.y, ok));
  std::vector<pi::Contact> out;
  for (std::size_t k = 0; k < footprints.size(); k++) {
    std::vector<std::pair<std::size_t, double>> sources;
    for (const auto& [x, y] : samples(footprints[k].xy, step)) {
      const auto j = access::join(metres, x, y, ok);
      if (j.gap <= reach) sources.push_back({j.point, j.gap});
    }
    if (sources.empty()) continue;
    const auto sp = morphe::combinatorics::dijkstra(metres.arcs, sources, reach);
    for (std::size_t i = 0; i < places.size(); i++) {
      const double d = sp.distance[at[i].point] + at[i].gap;
      if (d <= reach) out.push_back({i, k, d});
    }
  }
  return out;
}

struct UnitEffect {
  std::size_t unit = 0;
  std::int64_t onset = 0;         // the period it entered its ring
  int ring = 0;                   // its entry ring
  double effect = std::numeric_limits<double>::quiet_NaN();  // δ_i
  std::size_t before = 0, after = 0, comparisons = 0;        // periods in each window, comparison units
};

// Each reached unit's own change across its entry, against the units not yet reached through the same periods (windows of `window`
// periods of the exposure). `unit` gives each observation's unit as an index of the exposure, `period` its label.
inline std::vector<UnitEffect> unit_effects(std::span<const double> y, std::span<const std::size_t> unit, std::span<const std::int64_t> period,
                                            const pi::Exposure& e, std::size_t window) {
  if (unit.size() != y.size() || period.size() != y.size()) throw std::invalid_argument("interventions: a unit and a period for every observation");
  std::vector<std::map<std::int64_t, double>> series(e.units);
  for (std::size_t k = 0; k < y.size(); k++) {
    if (unit[k] >= e.units) throw std::invalid_argument("interventions: units of the exposure");
    series[unit[k]][period[k]] = y[k];
  }
  const auto mean = [&](std::size_t i, std::size_t t0, std::size_t t1, std::size_t& n) {
    double s = 0;
    n = 0;
    for (std::size_t t = t0; t < t1 && t < e.periods.size(); t++) {
      const auto it = series[i].find(e.periods[t]);
      if (it != series[i].end()) s += it->second, n++;
    }
    return n ? s / static_cast<double>(n) : std::numeric_limits<double>::quiet_NaN();
  };
  std::vector<UnitEffect> out;
  for (std::size_t i = 0; i < e.units; i++) {
    if (e.reached(i) == pi::never) continue;
    UnitEffect u;
    u.unit = i, u.onset = e.reached(i), u.ring = e.entry(i);
    const std::size_t g = e.index(u.onset);
    if (g == 0) { out.push_back(u); continue; }
    const std::size_t a = g >= window ? g - window : 0, b = g + window;
    const double pre = mean(i, a, g, u.before), post = mean(i, g, b, u.after);
    // Those not reached by the window's last period.
    const std::int64_t last = e.periods[std::min(b, e.periods.size()) - 1];
    double change = 0;
    for (std::size_t c = 0; c < e.units; c++) {
      if (c == i || (e.reached(c) != pi::never && e.reached(c) <= last)) continue;
      std::size_t n0 = 0, n1 = 0;
      const double c0 = mean(c, a, g, n0), c1 = mean(c, g, b, n1);
      if (n0 && n1) change += c1 - c0, u.comparisons++;
    }
    if (u.before && u.after && u.comparisons) u.effect = (post - pre) - change / static_cast<double>(u.comparisons);
    out.push_back(u);
  }
  return out;
}

struct Observation {
  std::size_t unit = 0;      // a sensor's index
  std::int64_t period = 0;   // its label
  double value = 0;          // the outcome (a logarithm of counts, as a rule: effects then read as proportions)
};

struct Options {
  std::vector<double> bounds{100, 400};  // the rings' outer bounds, metres along the ways
  double step = 25;                      // the footprints' sampling, metres
  std::size_t window = 2;                // periods each side of a sensor's entry, for its own change
  std::size_t replications = 199;        // bootstrap draws of sensors, for the staggered estimates' errors
  std::uint64_t seed = 1;
  peira::quasi::Comparison comparison = peira::quasi::Comparison::not_yet_treated;
};

struct Evaluation {
  pi::Exposure exposure;
  std::vector<pi::Contact> contacts;
  pi::RingFit two_way;                             // τ_r in the two-way regression
  std::vector<peira::quasi::StaggeredFit> by_ring;  // per ring r < R: entering it, against sensors not yet reached
  std::vector<UnitEffect> units;
  std::vector<std::size_t> entering;               // per ring r ≤ R: the sensors entering it (R: never reached)
};

// The chain on a city's records: the contacts along the ways, the exposure of the periods given, the effects by ring in the two-way
// regression and by staggered adoption, and each sensor's own change.
inline Evaluation evaluate(const access::Network& metres, const std::vector<Place>& places, const std::vector<Footprint>& footprints,
                           const std::vector<Observation>& observations, const std::vector<std::int64_t>& periods, const Options& o = {}) {
  if (o.bounds.empty()) throw std::invalid_argument("interventions: one ring at least");
  Evaluation ev;
  ev.contacts = contacts(metres, places, footprints, o.bounds.back(), o.step);
  std::vector<std::int64_t> begin;
  for (const auto& f : footprints) begin.push_back(f.begin);
  ev.exposure = pi::expose(places.size(), periods, begin, ev.contacts, o.bounds);
  std::vector<double> y;
  std::vector<std::size_t> unit;
  std::vector<std::int64_t> label, period;
  for (const auto& ob : observations) {
    if (ob.unit >= places.size()) throw std::invalid_argument("interventions: observations of the places given");
    y.push_back(ob.value), unit.push_back(ob.unit), label.push_back(static_cast<std::int64_t>(ob.unit)), period.push_back(ob.period);
  }
  const std::size_t R = o.bounds.size();
  ev.entering.assign(R + 1, 0);
  for (std::size_t i = 0; i < places.size(); i++) ev.entering[static_cast<std::size_t>(ev.exposure.entry(i))]++;
  ev.two_way = pi::ring_two_way(y, label, period, pi::rings_of(ev.exposure, unit, period), R);
  calx::random::Stream st(o.seed, 0x696e7476u, 0, 0);  // "intv"
  for (std::size_t r = 0; r < R; r++) ev.by_ring.push_back(pi::ring_staggered(y, unit, period, ev.exposure, r, o.comparison, o.replications, &st));
  ev.units = unit_effects(y, unit, period, ev.exposure, o.window);
  return ev;
}

}  // namespace urban::interventions
