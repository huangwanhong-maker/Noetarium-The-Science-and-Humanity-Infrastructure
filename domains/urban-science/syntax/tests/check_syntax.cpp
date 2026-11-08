// Checks of urban-science's syntax: streets as geometric graphs, blocks as 2-complexes.
//
// - Four streets each way, drawn as polylines with a point every 10 m, become a graph of their 16 junctions and 24 segments: the
//   straight runs of points between junctions are one edge each.
// - A way that turns a corner keeps the corner; a ring of points alone keeps one point for itself.
// - The grid's 9 blocks are 18 triangles whose areas sum to the blocks' (9 × 100 m × 100 m).
// - The grid of streets has a grammar: it compresses, and its replay rebuilds the junctions and segments exactly.
// - Configuration (ADR 0084): along a straight street angular choice is a path's betweenness and every angular depth 0; NAIN and
//   NACH by their formulas; at a T the stem is a right angle (cost 1) from either arm, and topologically the arms are one line; the
//   grid's integration and choice are unchanged by its quarter turn, its inner segments carry more choice, and the answer is the same
//   on one thread and on four; a metric radius of 150 m reaches only the segments meeting each one; the nearest segment to a point.

#include <urban/syntax/configuration.hpp>
#include <urban/syntax/syntax.hpp>

#include <syntaxis/derivation/derivation.hpp>
#include <syntaxis/induction/induction.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <string>
#include <vector>

int main() {
  calx::verification::Suite s;
  std::vector<std::vector<double>> ways;
  for (int k = 0; k < 4; k++) {
    std::vector<double> h, v;
    for (int i = 0; i <= 30; i++) {
      h.push_back(i * 10.0), h.push_back(k * 100.0);
      v.push_back(k * 100.0), v.push_back(i * 10.0);
    }
    ways.push_back(h), ways.push_back(v);
  }
  const auto g = urban::syntax::street_graph(ways);
  s.require("street_graph: four streets each way are 16 junctions and 24 segments", g.size() == 16 && g.edge_count() == 24,
            std::to_string(g.size()) + " nodes, " + std::to_string(g.edge_count()) + " edges");
  const auto corner = urban::syntax::street_graph({{0, 0, 10, 0, 20, 0, 20, 10, 20, 20}});
  s.require("street_graph: a way turning a corner keeps the corner (three nodes, two segments)", corner.size() == 3 && corner.edge_count() == 2,
            std::to_string(corner.size()) + " nodes");
  std::vector<double> ring;
  for (int k = 0; k <= 12; k++) ring.push_back(10 * std::cos(2 * std::numbers::pi * k / 12)), ring.push_back(10 * std::sin(2 * std::numbers::pi * k / 12));
  const auto round = urban::syntax::street_graph({ring}, 1.0, 45);
  s.require("street_graph: a ring of points alone keeps one point for itself", round.size() == 1, std::to_string(round.size()) + " nodes");

  const auto c = urban::syntax::blocks(g);
  double area = 0;
  for (const auto& t : c.triangles) area += syntaxis::simplicial::detail::orient(c.points[t[0]], c.points[t[1]], c.points[t[2]]) / 2;
  s.require("blocks: the grid's 9 blocks are 18 triangles", c.triangles.size() == 18, std::to_string(c.triangles.size()));
  s.check("blocks: their areas sum to 9 × 100 m × 100 m", area - 90000, 1e-6);

  const auto L = syntaxis::induction::learn(g);
  const auto r = syntaxis::derivation::replay(L, g);
  s.require("the street grid's grammar: rules found, each used at least twice", !L.grammar.rules.empty() && L.grammar.rules[0].uses >= 2, std::to_string(L.grammar.rules.size()) + " rules");
  s.require("its replay rebuilds the segments exactly", r.graph.edges() == g.edges());
  s.check("and the junctions (relative to the extent)", r.worst, 1e-9);

  // ---- configuration ----------------------------------------------------------------------------------------------------
  using urban::syntax::Cost;
  {
    // A straight street of five segments: no turn costs anything, so angular least paths are the metric ones and choice is the
    // path's betweenness i(4 − i); every angular total depth is 0.
    syntaxis::graphs::Graph straight;
    for (int k = 0; k <= 5; k++) straight.add({k * 100.0, 0});
    for (int k = 0; k < 5; k++) straight.connect(k, k + 1);
    const auto m = urban::syntax::segment_map(straight);
    const auto c = urban::syntax::configuration(m, Cost::angular);
    double worst = 0, depth = 0;
    for (std::size_t i = 0; i < 5; i++) worst = std::max(worst, std::fabs(c.choice[i] - static_cast<double>(i * (4 - i)))), depth += c.total_depth[i];
    s.check("configuration: along a straight street, angular choice is i(4 − i)", worst, 1e-12);
    s.check("and every angular total depth is 0", depth, 1e-12);
    s.check("NACH = log(CH + 1)/log(TD + 3) and NAIN = NC^1.2/(TD + 2), on the middle segment",
            std::fabs(c.nach[2] - std::log(5.0) / std::log(3.0)) + std::fabs(c.integration[2] - std::pow(4.0, 1.2) / 2), 1e-12);
  }
  {
    // A T: a right turn costs 1, going straight on 0, a reversal is not a join.
    syntaxis::graphs::Graph t;
    t.add({0, 0}), t.add({100, 0}), t.add({200, 0}), t.add({100, 100});
    t.connect(0, 1), t.connect(1, 2), t.connect(1, 3);
    const auto m = urban::syntax::segment_map(t);
    const auto c = urban::syntax::configuration(m, Cost::angular);
    // Segments: 0 = (0,1), 1 = (1,2), 2 = (1,3). From 0: segment 1 straight on (0), segment 2 a right angle (1).
    s.check("configuration: at a T, the stem is a right angle from either arm (angular total depth 1 + 0)", c.total_depth[0] - 1, 1e-12);
    s.check("and the stem's own total depth is 1 + 1", c.total_depth[2] - 2, 1e-12);
    const auto top = urban::syntax::configuration(m, Cost::topological, std::numeric_limits<double>::infinity(), 1, 30);
    s.check("topologically, the arms are one line (depth 0 between them) and the stem one step from it", top.total_depth[0] - 1, 1e-12);
  }
  {
    // The grid of four streets each way: its configuration is symmetric under the grid's quarter turn about its centre, the inner
    // segments carry more choice than the outer, and the answer does not depend on the threads.
    const auto m = urban::syntax::segment_map(g);
    const auto one = urban::syntax::configuration(m, Cost::angular, std::numeric_limits<double>::infinity(), 1);
    const auto four = urban::syntax::configuration(m, Cost::angular, std::numeric_limits<double>::infinity(), 4);
    bool same = true;
    for (std::size_t i = 0; i < m.size(); i++) same = same && one.choice[i] == four.choice[i] && one.total_depth[i] == four.total_depth[i];
    s.require("configuration: the same on one thread and on four, to the bit", same);
    const auto centre = syntaxis::shape::Point{150, 150};
    double asym = 0, inner = 0, outer = 0;
    int ni = 0, no = 0;
    for (std::size_t i = 0; i < m.size(); i++) {
      const auto mid = (m.a[i] + m.b[i]) / 2.0;
      const auto turned = centre + (mid - centre) * syntaxis::shape::Point{0, 1};
      const auto near = urban::syntax::nearest_segment(m, turned);
      asym = std::max(asym, std::fabs(one.choice[near.segment] - one.choice[i]) + std::fabs(one.integration[near.segment] - one.integration[i]));
      const bool edge = std::fabs(mid.real()) < 1 || std::fabs(mid.real() - 300) < 1 || std::fabs(mid.imag()) < 1 || std::fabs(mid.imag() - 300) < 1;
      (edge ? outer : inner) += one.choice[i];
      (edge ? no : ni) += 1;
    }
    s.check("the grid's integration and choice are unchanged by its quarter turn", asym, 1e-9);
    s.require("its inner segments carry more choice than its outer ones, on average", inner / ni > outer / no,
              std::to_string(inner / ni) + " against " + std::to_string(outer / no));
    // A metric radius of 150 m reaches only the segments meeting each segment (centres 100 m apart); 250 m reaches more.
    const auto near = urban::syntax::configuration(m, Cost::metric, 150);
    const auto wide = urban::syntax::configuration(m, Cost::metric, 250);
    bool degree = true, more = false;
    for (std::size_t i = 0; i < m.size(); i++) degree = degree && near.node_count[i] == static_cast<double>(m.joins[i].size()), more = more || wide.node_count[i] > near.node_count[i];
    s.require("a metric radius of 150 m reaches the segments meeting each one; 250 m reaches more", degree && more);
    const auto p = urban::syntax::nearest_segment(m, {50, 7});
    s.require("nearest_segment: a point 7 m from the first street's first segment", std::fabs(p.distance - 7) < 1e-9 && ((m.a[p.segment] + m.b[p.segment]) / 2.0 == syntaxis::shape::Point{50, 0}));
  }
  return s.done();
}
