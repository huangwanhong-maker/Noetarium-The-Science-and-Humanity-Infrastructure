// Checks of urban-science's syntax: streets as geometric graphs, blocks as 2-complexes.
//
// - Four streets each way, drawn as polylines with a point every 10 m, become a graph of their 16 junctions and 24 segments: the
//   straight runs of points between junctions are one edge each.
// - A way that turns a corner keeps the corner; a ring of points alone keeps one point for itself.
// - The grid's 9 blocks are 18 triangles whose areas sum to the blocks' (9 × 100 m × 100 m).
// - The grid of streets has a grammar: it compresses, and its replay rebuilds the junctions and segments exactly.

#include <urban/syntax/syntax.hpp>

#include <syntaxis/derivation/derivation.hpp>
#include <syntaxis/induction/induction.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
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
  return s.done();
}
