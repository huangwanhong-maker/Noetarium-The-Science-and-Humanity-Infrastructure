// Who can reach what: places of demand and of supply joined to the network of a place's ways, the pairs within a time, the time to the
// nearest supply, and the distance to a road (ADR 0086). The measures of accessibility made of them (cumulative opportunities,
// gravity, the two-step floating catchment, unmet need, the Rural Access Index) are nomos's.
//
// A place joins the network at its nearest point in a connected part of it holding at least 1% of the points joined to anything (and
// always in the largest part), the metres between walked at the walking speed: a cell of people at its centre, a building at its
// centroid. A way closed to a mode (a footway to a car) leaves its stubs, stitched at their loose ends, as islands of a few points;
// a place joined to one of them would reach nothing, however near the rest of the network. The time of a pair is that walk at each end and the least time over the network
// between (morphe's Dijkstra from each supply, bounded by the limit). Times are in seconds. As in access.hpp, a time is a lower bound
// on a journey: no signals, gradients or waiting.

#pragma once

#include <urban/access/access.hpp>

#include <nomos/measurement/accessibility.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace urban::access {

struct Joined {
  std::size_t point = 0;  // the network's point
  double gap = 0;         // metres walked to it
};

// The points a place may join: those in a connected part of the network holding at least `share` of the points joined to anything,
// and those of the largest part.
inline std::vector<char> joinable(const Network& n, double share = 0.01) {
  const std::size_t N = n.x.size();
  std::vector<std::size_t> parent(N);
  std::iota(parent.begin(), parent.end(), std::size_t{0});
  const auto find = [&](std::size_t a) {
    while (parent[a] != a) a = parent[a] = parent[parent[a]];
    return a;
  };
  for (std::size_t p = 0; p < N; p++)
    for (const auto& arc : n.arcs[p]) {
      const std::size_t a = find(p), b = find(arc.to);
      if (a != b) parent[a] = b;
    }
  std::vector<std::size_t> size(N, 0);
  std::size_t joined = 0, largest = 0;
  for (std::size_t p = 0; p < N; p++)
    if (!n.arcs[p].empty()) joined++, largest = std::max(largest, ++size[find(p)]);
  const double least = std::min(static_cast<double>(largest), share * static_cast<double>(joined));
  std::vector<char> ok(N, 0);
  for (std::size_t p = 0; p < N; p++) ok[p] = !n.arcs[p].empty() && static_cast<double>(size[find(p)]) >= least;
  return ok;
}

// The nearest point of the network that a place may join.
inline Joined join(const Network& n, double x, double y, const std::vector<char>& ok) {
  Joined best{0, std::numeric_limits<double>::infinity()};
  for (std::size_t p = 0; p < n.x.size(); p++) {
    if (!ok[p]) continue;
    const double d = std::hypot(n.x[p] - x, n.y[p] - y);
    if (d < best.gap) best = {p, d};
  }
  if (!std::isfinite(best.gap)) throw std::invalid_argument("access: the network joins nothing");
  return best;
}

// The pairs (demand i, supply j) within `limit` seconds, each with its time; from a bounded search from each supply.
inline std::vector<nomos::measurement::Reachable> pairs_within(const Network& n, const std::vector<std::pair<double, double>>& demand,
                                                               const std::vector<std::pair<double, double>>& supply, double limit, double walk = 1.34) {
  const auto ok = joinable(n);
  std::vector<Joined> dj, sj;
  for (const auto& [x, y] : demand) dj.push_back(join(n, x, y, ok));
  for (const auto& [x, y] : supply) sj.push_back(join(n, x, y, ok));
  // The demand at each point, to read a search's distances once per point.
  std::vector<std::vector<std::size_t>> at(n.x.size());
  for (std::size_t i = 0; i < dj.size(); i++) at[dj[i].point].push_back(i);
  std::vector<nomos::measurement::Reachable> out;
  for (std::size_t j = 0; j < sj.size(); j++) {
    const double start = sj[j].gap / walk;
    if (start > limit) continue;
    const auto sp = mc::dijkstra(n.arcs, {{sj[j].point, start}}, limit);
    for (std::size_t p = 0; p < n.x.size(); p++) {
      if (at[p].empty() || !std::isfinite(sp.distance[p])) continue;
      for (const std::size_t i : at[p]) {
        const double t = sp.distance[p] + dj[i].gap / walk;
        if (t <= limit) out.push_back({i, j, t});
      }
    }
  }
  return out;
}

// The time from each place of demand to its nearest supply (+∞ where none is reached), by one search from all supplies at once.
inline std::vector<double> nearest_supply(const Network& n, const std::vector<std::pair<double, double>>& demand, const std::vector<std::pair<double, double>>& supply,
                                          double walk = 1.34) {
  const auto ok = joinable(n);
  std::vector<std::pair<std::size_t, double>> sources;
  for (const auto& [x, y] : supply) {
    const auto s = join(n, x, y, ok);
    sources.push_back({s.point, s.gap / walk});
  }
  const auto sp = mc::dijkstra(n.arcs, sources);
  std::vector<double> t;
  for (const auto& [x, y] : demand) {
    const auto d = join(n, x, y, ok);
    t.push_back(sp.distance[d.point] + d.gap / walk);
  }
  return t;
}

// The straight-line distance from a point to the nearest of some polylines (x0, y0, x1, y1, …), metres; +∞ when there are none.
inline double distance_to_lines(double x, double y, const std::vector<std::vector<double>>& lines) {
  double best = std::numeric_limits<double>::infinity();
  for (const auto& l : lines)
    for (std::size_t i = 0; i + 3 < l.size(); i += 2) {
      const double ax = l[i], ay = l[i + 1], bx = l[i + 2], by = l[i + 3], dx = bx - ax, dy = by - ay, L2 = dx * dx + dy * dy;
      const double t = L2 > 0 ? std::clamp(((x - ax) * dx + (y - ay) * dy) / L2, 0.0, 1.0) : 0.0;
      best = std::min(best, std::hypot(x - ax - t * dx, y - ay - t * dy));
    }
  return best;
}

}  // namespace urban::access
