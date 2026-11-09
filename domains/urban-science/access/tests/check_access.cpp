// Checks of reach over a network of ways (access/access.hpp).
//
// - Ways meeting at a shared point are joined; ways that only cross, with no shared point (a bridge), are not.
// - A grid of streets of known spacing at walking speed: every time is the Manhattan distance over the speed.
// - The walk to the network counts, and it enters at the nearest point.
// - A faster way is taken when it is faster, a closed way never.
// - The length within a time: the whole network at a long limit, a known part at a short one, proportionately along a piece.

#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace ua = urban::access;

int main() {
  calx::verification::Suite s;
  {
    const auto n = ua::network_of({{{0, 0, 100, 0}, 1}, {{100, 0.2, 100, 100}, 1}, {{50, -50, 50, 50}, 1}});
    s.require("ways sharing a point within the snapping distance are joined", n.x.size() == 5, std::to_string(n.x.size()));
    const auto r = ua::reach(n, {{0, 0}}, 1);
    s.require("one reaches along them", std::isfinite(r.time[n.points[1][1]]));
    s.require("a way that only crosses them, sharing no point, is not joined", !std::isfinite(r.time[n.points[2][0]]));
  }
  {
    std::vector<ua::Way> ways;
    const int m = 9;
    const double d = 120;
    for (int i = 0; i < m; i++) {
      ua::Way a, b;
      a.speed = b.speed = 1.5;
      for (int j = 0; j < m; j++) { a.xy.push_back(i * d); a.xy.push_back(j * d); b.xy.push_back(j * d); b.xy.push_back(i * d); }
      ways.push_back(a);
      ways.push_back(b);
    }
    const auto n = ua::network_of(ways);
    const auto r = ua::reach(n, {{4 * d, 4 * d}}, 1.5);
    double worst = 0;
    for (std::size_t p = 0; p < n.x.size(); p++) worst = std::max(worst, std::abs(r.time[p] - (std::abs(n.x[p] - 4 * d) + std::abs(n.y[p] - 4 * d)) / 1.5));
    s.check("on a grid of streets every time is the Manhattan distance over the speed", worst, 1e-9);
    const auto within = ua::length_within(n, r.time, {1e6, 120 / 1.5, 60 / 1.5});
    const double total = 2.0 * m * (m - 1) * d;
    s.check("at a long limit the whole network is within reach", within[0] / total - 1, 1e-12);
    s.check("within one block, the four arms from the crossing", within[1] / (4 * d) - 1, 1e-12);
    s.check("within half a block, in proportion along them", within[2] / (2 * d) - 1, 1e-12);
    const auto far = ua::reach(n, {{4 * d - 50, 4 * d}}, 1.0);
    s.check("the walk to the nearest point counts, at its own speed", far.time[far.start] - 50, 1e-9);
    s.check("and it is reported", far.start_gap - 50, 1e-9);
    const auto L = ua::reach(n, {{0, 0}}, 1.5, 200);
    int reached = 0;
    for (const double t : L.time) reached += std::isfinite(t);
    s.require("a limit stops the search", reached == 6, std::to_string(reached));
  }
  {
    // A slow straight way, and a fast way round it.
    const auto n = ua::network_of({{{0, 0, 300, 0}, 1}, {{0, 0, 0, 100, 300, 100, 300, 0}, 10}});
    const auto r = ua::reach(n, {{0, 0}}, 1);
    s.check("the faster way round is taken", r.time[n.points[0][1]] - 500.0 / 10, 1e-9);
    const auto closed = ua::network_of({{{0, 0, 300, 0}, 1}, {{0, 0, 0, 100, 300, 100, 300, 0}, 0}});
    const auto c = ua::reach(closed, {{0, 0}}, 1);
    s.check("a closed way is never taken", c.time[closed.points[0][1]] - 300, 1e-9);
  }
  {
    // Two ways cut at a tile's edge, 3 m apart at their ends.
    auto n = ua::network_of({{{0, 0, 100, 0}, 1}, {{103, 0, 200, 0}, 1}});
    s.require("ways cut apart are not joined at first", !std::isfinite(ua::reach(n, {{0, 0}}, 1).time[n.points[1][1]]));
    s.require("their loose ends are stitched within the radius", ua::stitch_ends(n, 8, 1) >= 1);
    s.check("and one walks across at the stitching's speed", ua::reach(n, {{0, 0}}, 1).time[n.points[1][1]] - 200, 1e-9);
    auto m = ua::network_of({{{0, 0, 100, 0}, 1}, {{120, 0, 200, 0}, 1}});
    s.require("beyond the radius they stay apart", ua::stitch_ends(m, 8, 1) == 0);
  }
  {
    // Catchments (catchment.hpp): a straight road of 2 km at 1 m/s, a clinic at its west end, two places of people beside it.
    const auto n = ua::network_of({{{0, 0, 1000, 0, 2000, 0}, 1}});
    const std::vector<std::pair<double, double>> people{{600, 30}, {1800, 0}}, clinic{{0, 40}};
    const auto pairs = ua::pairs_within(n, people, clinic, 1500, 1);
    // The network's points are the road's vertices (0, 1000, 2000). The first place joins (1000, 0), 401.1 m away; the clinic joins
    // (0, 0), 40 m away; 1000 s between: 1441.1 s. The second joins (2000, 0), 200 m away: 2240 s, beyond the limit.
    s.require("catchment: within 1,500 s only the nearer place reaches the clinic", pairs.size() == 1 && pairs[0].demand == 0, std::to_string(pairs.size()) + " pairs");
    s.check("... its time is the walks to the network and the way between", pairs[0].cost - (std::hypot(400, 30) + 1000 + 40), 1e-9);
    const auto t = ua::nearest_supply(n, people, clinic, 1);
    s.check("the time to the nearest clinic, for the farther place", t[1] - (200 + 2000 + 40), 1e-9);
    const auto f = nomos::measurement::two_step(ua::pairs_within(n, people, clinic, 5000, 1), std::vector<double>{300, 700}, std::vector<double>{50},
                                                nomos::measurement::Catchment{5000});
    s.check("with both in reach, the two-step catchment shares the clinic: 50 / 1,000 a person each", std::fabs(f.access[0] - 0.05) + std::fabs(f.access[1] - 0.05), 1e-15);
    s.check("the distance to a road is the perpendicular to its nearest segment", ua::distance_to_lines(500, 30, {{0, 0, 1000, 0}}) - 30, 1e-12);
  }
  {
    // A street drawn as one segment of 400 m, and a side street that ends 5 m short of its middle (a tile dropped the vertex they
    // shared): stitch_ends finds no point within 8 m; stitch_to_ways joins it to the street's middle, splitting the street.
    auto n = ua::network_of({{{0, 0, 400, 0}, 1}, {{200, 105, 200, 5}, 1}});
    s.require("a side street ending 5 m from a bare segment: no point within 8 m", ua::stitch_ends(n, 8, 1) == 0);
    s.require("... stitch_to_ways joins it, splitting the street", ua::stitch_to_ways(n, 8, 1) == 1 && n.x.size() == 5);
    const auto r = ua::reach(n, {{200, 105}}, 1);
    s.check("... so from the side street's far end the street's end is 100 + 5 + 200 m away", r.time[n.points[0][1]] - 305, 1e-9);
  }
  {
    // An island: a road of 300 vertices 10 m apart, and beside a place a stub of two points joined to nothing else (a footway's ends
    // stitched, with the footway closed to the mode). The place is nearer the stub, but joins the road and reaches the clinic.
    std::vector<double> road;
    for (int i = 0; i < 300; i++) road.push_back(10.0 * i), road.push_back(0);
    const auto n = ua::network_of({{road, 1}, {{1500, 20, 1500, 25}, 1}});
    const auto ok = ua::joinable(n);
    s.require("an island of 2 points among 302 may not be joined", !ok[n.points[1][0]] && ok[n.points[0][150]]);
    const auto t = ua::nearest_supply(n, {{1500, 22}}, {{0, 0}}, 1);
    s.check("... so the place beside it reaches the clinic along the road: 22 m and 1,500 m", t[0] - 1522, 1e-9);
  }
  return s.done();
}
