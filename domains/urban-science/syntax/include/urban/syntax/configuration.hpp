// The configuration of streets: how each street segment stands in the whole network, after Space Syntax (Hillier and Hanson 1984,
// The Social Logic of Space; Hillier 1996, Space is the Machine), as centralities of the segment map (ADR 0084).
//
// - **The segment map.** Each edge of the street graph (street_graph: junctions, dead ends and bends) is a segment; two segments
//   meeting at a node are joined, both ways. Passing from one to the other turns by the angle θ ∈ [0, π] between their directions
//   of travel, and covers half of each (their centres' distance along the street).
// - **Costs.** Metric: the metres from centre to centre. Angular: θ/(π/2), so that a right angle costs 1 and a reversal 2 (Turner
//   2001, "Angular analysis", 3rd International Space Syntax Symposium; Hillier and Iida 2005, COSIT 2005, LNCS 3693, 475–490).
//   Topological: 1 for a turn of more than `turn` degrees, 0 for going on, so that a street running straight through junctions is
//   one step (a declared threshold; continuity lines in the sense of Figueiredo and Amorim 2005). Least paths are least in cost,
//   then in metres (morphe's centrality), and the radius is in metres along them; radius n is the whole network.
// - **Measures.** For each segment: the segments reached within the radius (node count, NC), the sum of least costs to them (total
//   depth, TD), their mean (MD = TD/NC); choice (CH), the betweenness of least paths over pairs of segments, each pair once; and
//   Hillier, Yang and Turner's (2012, J. Space Syntax 3, 155–193) normalisations for angular analysis, NAIN = NC^1.2/(TD + 2)
//   (integration) and NACH = log(CH + 1)/log(TD + 3) (choice), which make networks of different sizes comparable.
//
// Limits: a segment is the chord between its ends (bends under street_graph's threshold are straightened); ways that cross
// without meeting (a bridge) are joined where their points meet only; the network's edge cuts the paths that would leave it, so
// measures near the edge of what was read are lower than the city's (an edge effect, smaller at smaller radii).

#pragma once

#include <urban/syntax/syntax.hpp>

#include <calx/scheduler/parallel_for.hpp>
#include <calx/scheduler/thread_pool.hpp>
#include <morphe/combinatorics/centrality.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace urban::syntax {

enum class Cost { metric, angular, topological };

struct SegmentMap {
  std::vector<std::array<std::size_t, 2>> ends;  // the street graph's nodes at each end
  std::vector<Point> a, b;                       // their positions
  std::vector<double> length;                    // metres
  struct Join {
    std::size_t to = 0;
    double turn = 0;                             // radians, in [0, π]
    double metres = 0;                           // centre to centre
  };
  std::vector<std::vector<Join>> joins;
  std::size_t size() const { return ends.size(); }
};

// The segment map of a geometric graph: its edges, joined where they meet.
inline SegmentMap segment_map(const Graph& g) {
  SegmentMap m;
  std::vector<std::vector<std::size_t>> at(g.size());  // the segments at each node
  for (const auto& [u, v] : g.edges()) {
    const std::size_t s = m.ends.size();
    m.ends.push_back({u, v});
    m.a.push_back(g.nodes[u].at), m.b.push_back(g.nodes[v].at);
    m.length.push_back(std::abs(g.nodes[v].at - g.nodes[u].at));
    at[u].push_back(s), at[v].push_back(s);
  }
  m.joins.assign(m.size(), {});
  for (std::size_t v = 0; v < g.size(); v++)
    for (const std::size_t i : at[v])
      for (const std::size_t j : at[v]) {
        if (i == j) continue;
        // Travel along i towards v, then along j away from it.
        const Point p = m.ends[i][0] == v ? m.b[i] : m.a[i];
        const Point q = m.ends[j][0] == v ? m.b[j] : m.a[j];
        const Point here = g.nodes[v].at, in = here - p, out = q - here;
        const double turn = (std::abs(in) > 0 && std::abs(out) > 0) ? std::abs(std::arg(out / in)) : 0.0;
        m.joins[i].push_back({j, turn, (m.length[i] + m.length[j]) / 2});
      }
  return m;
}

// The segment map as a graph of costs and lengths, for morphe's centrality. A join of zero metres (two segments of no length)
// takes a millimetre, as least paths need lengths that are positive.
inline morphe::combinatorics::CostAdjacency cost_graph(const SegmentMap& m, Cost cost, double turn = 30) {
  morphe::combinatorics::CostAdjacency g(m.size());
  const double threshold = turn * std::numbers::pi / 180;
  for (std::size_t i = 0; i < m.size(); i++)
    for (const auto& j : m.joins[i]) {
      const double metres = std::max(j.metres, 1e-3);
      const double c = cost == Cost::metric ? metres : cost == Cost::angular ? j.turn / (std::numbers::pi / 2) : (j.turn > threshold ? 1.0 : 0.0);
      g[i].push_back({j.to, c, metres});
    }
  return g;
}

struct Configuration {
  Cost cost = Cost::angular;
  double radius = std::numeric_limits<double>::infinity();
  std::vector<double> node_count, total_depth, mean_depth, choice, integration, nach;  // per segment; integration is NAIN
};

// The configuration at one cost and radius, the sources spread over `threads` (the result does not depend on how many).
inline Configuration configuration(const SegmentMap& m, Cost cost, double radius = std::numeric_limits<double>::infinity(), std::size_t threads = 1,
                                   double turn = 30) {
  if (!(radius > 0)) throw std::invalid_argument("urban::syntax::configuration: a radius > 0");
  const auto g = cost_graph(m, cost, turn);
  const std::size_t n = m.size(), grain = 32, chunks = (n + grain - 1) / grain;
  morphe::combinatorics::CentralityOptions o;
  o.radius = radius, o.undirected = true;
  // Each chunk of sources keeps its own sums, added in the chunks' order, so that the result is the same on any number of threads.
  std::vector<morphe::combinatorics::Centrality> part(chunks);
  calx::scheduler::ThreadPool pool(std::max<std::size_t>(1, threads));
  calx::scheduler::parallel_for(pool, n, grain, [&](std::size_t begin, std::size_t end, std::size_t) {
    std::vector<std::size_t> sources;
    for (std::size_t s = begin; s < end; s++) sources.push_back(s);
    part[begin / grain] = morphe::combinatorics::centrality(g, sources, o);
  });
  Configuration c;
  c.cost = cost, c.radius = radius;
  c.node_count.assign(n, 0), c.total_depth.assign(n, 0), c.choice.assign(n, 0);
  for (const auto& p : part)
    for (std::size_t v = 0; v < n; v++) c.node_count[v] += p.reached[v], c.total_depth[v] += p.total_cost[v], c.choice[v] += p.betweenness[v];
  c.mean_depth.resize(n), c.integration.resize(n), c.nach.resize(n);
  for (std::size_t v = 0; v < n; v++) {
    c.mean_depth[v] = c.node_count[v] > 0 ? c.total_depth[v] / c.node_count[v] : 0;
    c.integration[v] = std::pow(c.node_count[v], 1.2) / (c.total_depth[v] + 2);
    c.nach[v] = std::log(c.choice[v] + 1) / std::log(c.total_depth[v] + 3);
  }
  return c;
}

// The segment nearest a point, and its distance (metres); none when the map is empty.
struct Nearest {
  std::size_t segment = std::numeric_limits<std::size_t>::max();
  double distance = std::numeric_limits<double>::infinity();
};
inline Nearest nearest_segment(const SegmentMap& m, Point p) {
  Nearest best;
  for (std::size_t s = 0; s < m.size(); s++) {
    const Point d = m.b[s] - m.a[s];
    const double L2 = std::norm(d);
    const double t = L2 > 0 ? std::clamp(((p - m.a[s]) * std::conj(d)).real() / L2, 0.0, 1.0) : 0.0;
    const double r = std::abs(p - (m.a[s] + t * d));
    if (r < best.distance) best = {s, r};
  }
  return best;
}

}  // namespace urban::syntax
