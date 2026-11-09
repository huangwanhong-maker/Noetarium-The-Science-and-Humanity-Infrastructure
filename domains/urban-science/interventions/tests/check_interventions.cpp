// Checks of interventions (interventions.hpp).
//
// - Distances along the network, by hand: around a corner a sensor is 1,400 m from a footprint 1,000 m away over the ground; a
//   sensor beside a way joins it; a footprint beyond the reach makes no contact.
// - A city of streets every 100 m, 40 sensors (six on the streets that change), three street changes beginning in periods 4, 6 and 9 of 12, a direct effect of 5
//   within 60 m along the streets and a spillover of 2 within 300 m, without noise: peira's estimators on these contacts recover both,
//   by staggered adoption and in a two-way regression; each sensor's own change is its ring's effect.

#include <urban/interventions/interventions.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace ua = urban::access;
namespace ui = urban::interventions;
namespace pi = peira::interference;

namespace {
// A straight way from (x0, y0) to (x1, y1) with vertices every `step` metres, at 1 m/s.
ua::Way way(double x0, double y0, double x1, double y1, double step) {
  ua::Way w;
  w.speed = 1;
  const int n = static_cast<int>(std::round(std::hypot(x1 - x0, y1 - y0) / step));
  for (int k = 0; k <= n; k++) w.xy.push_back(x0 + (x1 - x0) * k / n), w.xy.push_back(y0 + (y1 - y0) * k / n);
  return w;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    // An L of ways, vertices every 100 m.
    const auto n = ua::network_of({way(0, 0, 1000, 0, 100), way(1000, 0, 1000, 1000, 100)});
    const std::vector<ui::Place> sensors{{1000, 600}, {100, 30}};
    const std::vector<ui::Footprint> fp{{{200, 0}, 3}, {{0, 0, 400, 0}, 5}, {{0, 0}, 1}};
    const auto c = ui::contacts(n, sensors, fp, 1500);
    double far = -1, near = -1, line = -1;
    bool beyond = false;
    for (const auto& x : c) {
      if (x.unit == 0 && x.treatment == 0) far = x.distance;
      if (x.unit == 1 && x.treatment == 0) near = x.distance;
      if (x.unit == 0 && x.treatment == 1) line = x.distance;
      if (x.unit == 0 && x.treatment == 2) beyond = true;
    }
    s.check("contacts: around the corner, 800 + 600 m along the ways (1,000 m over the ground)", far - 1400, 1e-9);
    s.check("... a sensor 30 m beside a way joins it: 100 + 30 m", near - 130, 1e-9);
    s.check("... a lane's nearest point counts: 600 + 600 m", line - 1200, 1e-9);
    s.require("... and a footprint 1,600 m along the ways is beyond a reach of 1,500", !beyond);
  }
  {
    // A city of streets every 100 m over 1.2 km; sensors at 40 junctions; three street changes in its south-west.
    std::vector<ua::Way> ways;
    for (int k = 0; k <= 12; k++) ways.push_back(way(0, 100.0 * k, 1200, 100.0 * k, 25)), ways.push_back(way(100.0 * k, 0, 100.0 * k, 1200, 25));
    const auto n = ua::network_of(ways);
    calx::random::Stream st(20261109, 0x75726269u, 0, 0);  // "urbi"
    // Six sensors on the streets that change, 34 at junctions drawn at random.
    std::vector<ui::Place> sensors{{150, 200}, {300, 200}, {400, 150}, {400, 250}, {200, 500}, {200, 600}};
    for (int k = 0; k < 34; k++) sensors.push_back({100.0 * static_cast<double>(st.below(13)), 100.0 * static_cast<double>(st.below(13))});
    const std::vector<ui::Footprint> fp{{{100, 200, 300, 200}, 4}, {{400, 100, 400, 300}, 6}, {{200, 500, 200, 600}, 9}};
    std::vector<std::int64_t> begin;
    for (const auto& f : fp) begin.push_back(f.begin);
    std::vector<std::int64_t> periods;
    for (std::int64_t t = 1; t <= 12; t++) periods.push_back(t);
    const auto e = pi::expose(sensors.size(), periods, begin, ui::contacts(n, sensors, fp, 400), {60, 300});
    std::vector<double> alpha(sensors.size()), gamma(12);
    for (auto& a : alpha) a = st.uniform(3, 9);
    for (auto& g : gamma) g = st.uniform(-1, 1);
    std::vector<double> y;
    std::vector<std::size_t> unit;
    std::vector<std::int64_t> label, period;
    std::size_t counts[3] = {0, 0, 0};
    for (std::size_t i = 0; i < sensors.size(); i++) {
      counts[e.entry(i)]++;
      for (std::size_t t = 0; t < 12; t++) {
        const int r = e.at(i, t);
        y.push_back(alpha[i] + gamma[t] + (r == 0 ? 5.0 : r == 1 ? 2.0 : 0.0));
        unit.push_back(i), label.push_back(static_cast<std::int64_t>(i)), period.push_back(periods[t]);
      }
    }
    s.require("the city: sensors entering directly, by spillover and never reached: " + std::to_string(counts[0]) + ", " + std::to_string(counts[1]) + ", " +
                  std::to_string(counts[2]),
              counts[0] > 0 && counts[1] > 0 && counts[2] > 0);
    const auto d = pi::ring_staggered(y, unit, period, e, 0), sp = pi::ring_staggered(y, unit, period, e, 1);
    s.check("... by staggered adoption on the contacts along the streets, the direct effect 5 and the spillover 2", std::fabs(d.simple - 5) + std::fabs(sp.simple - 2), 1e-9);
    const auto f = pi::ring_two_way(y, label, period, pi::rings_of(e, unit, period), 2);
    s.check("... and in the two-way regression", std::fabs(f.effect[0] - 5) + std::fabs(f.effect[1] - 2), 1e-9);
    // Each sensor's own change, where its ring holds through the windows.
    double worst = 0;
    std::size_t seen = 0;
    for (const auto& u : ui::unit_effects(y, unit, period, e, 2)) {
      const std::size_t g = e.index(u.onset);
      if (g < 2 || g + 2 > 12 || !std::isfinite(u.effect)) continue;
      bool steady = true;
      for (std::size_t t = g; t < g + 2; t++) steady = steady && e.at(u.unit, t) == u.ring;
      if (!steady) continue;
      worst = std::max(worst, std::fabs(u.effect - (u.ring == 0 ? 5.0 : 2.0)));
      seen++;
    }
    s.check("... each sensor's own change across its entry is its ring's effect (" + std::to_string(seen) + " sensors)", worst, 1e-9);
    // The same through evaluate, in one call.
    std::vector<ui::Observation> obs;
    for (std::size_t k = 0; k < y.size(); k++) obs.push_back({unit[k], period[k], y[k]});
    ui::Options o;
    o.bounds = {60, 300};
    o.replications = 0;
    const auto ev = ui::evaluate(n, sensors, fp, obs, periods, o);
    s.check("... and through evaluate, in one call: both effects by both estimators",
            std::fabs(ev.by_ring[0].simple - 5) + std::fabs(ev.by_ring[1].simple - 2) + std::fabs(ev.two_way.effect[0] - 5) + std::fabs(ev.two_way.effect[1] - 2), 1e-9);
  }
  return s.done();
}
