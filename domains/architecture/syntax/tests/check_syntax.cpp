// Checks of architecture's syntax: footprints as 2-complexes, outlines as graphs.
//
// - Two buildings of 20 m × 10 m sharing a wall: their footprints are four triangles over six corners (the wall's two corners
//   shared), every triangle of the letter 90·60·30 at 15° (a 2 : 1 rectangle halved: 90°, 63.4°, 26.6°).
// - Their outlines are a graph of six nodes and seven edges, the shared wall counted once.
// - A row of identical houses has a gluing grammar of one letter, entropy zero.

#include <architecture/syntax/syntax.hpp>

#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using syntaxis::shape::Point;

int main() {
  calx::verification::Suite s;
  const std::vector<std::vector<Point>> two{{{0, 0}, {20, 0}, {20, 10}, {0, 10}}, {{20, 0}, {40, 0}, {40, 10}, {20, 10}}};
  const auto c = architecture::syntax::footprints(two);
  bool letters = c.triangles.size() == 4 && c.points.size() == 6;
  for (const auto& t : c.triangles) letters = letters && syntaxis::simplicial::name(syntaxis::simplicial::letter(c, t, 15), 15) == "90·60·30";
  s.require("footprints: two buildings sharing a wall are four triangles over six corners, each 90·60·30", letters,
            std::to_string(c.triangles.size()) + " triangles, " + std::to_string(c.points.size()) + " corners");
  const auto g = architecture::syntax::outlines(two);
  s.require("outlines: six nodes and seven edges, the shared wall once", g.size() == 6 && g.edge_count() == 7);
  std::vector<std::vector<Point>> row;
  for (int k = 0; k < 8; k++) row.push_back({{k * 6.0, 0}, {k * 6.0 + 6, 0}, {k * 6.0 + 6, 9}, {k * 6.0, 9}});
  const auto G = syntaxis::simplicial::learn_gluing(architecture::syntax::footprints(row), 15);
  s.require("a row of identical houses: one letter", G.letters.size() == 1);
  s.check("and the letters' entropy is zero", G.entropy, 1e-12);
  return s.done();
}
