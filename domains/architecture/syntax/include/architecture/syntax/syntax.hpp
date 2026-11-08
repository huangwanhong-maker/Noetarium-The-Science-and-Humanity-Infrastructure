// The syntax of buildings: footprints as 2-complexes and outlines as geometric graphs, for syntaxis's grammars (ADR 0083). The
// grammars are syntaxis's; what is the building's is here.
//
// - footprints: a district's footprints (rings in a local frame, metres), each triangulated by its constrained Delaunay
//   triangulation (physis, through syntaxis), as one 2-complex in which buildings that share a wall share its vertices (points within `merge` metres are one). Its
//   triangles are the district's words: their letters, the shapes of the pieces a footprint is made of.
// - outlines: the footprints' rings as one geometric graph, nodes at their corners, so that the graph grammars learn the shapes of
//   plans (the repeated corners, bays and courts of a district's buildings).
//
// Limits: a footprint is a simple ring (its courtyards, its inner rings, are not read yet); corners closer than `merge` merge.

#pragma once

#include <syntaxis/graphs/graphs.hpp>
#include <syntaxis/simplicial/simplicial.hpp>

#include <cmath>
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace architecture::syntax {

using syntaxis::graphs::Graph;
using syntaxis::shape::Point;

inline syntaxis::simplicial::Complex footprints(const std::vector<std::vector<Point>>& rings, double merge = 0.05) {
  return syntaxis::simplicial::from_rings(rings, merge);
}

inline Graph outlines(const std::vector<std::vector<Point>>& rings, double merge = 0.05) {
  Graph g;
  std::map<std::pair<std::int64_t, std::int64_t>, std::size_t> index;
  const auto node = [&](Point p) {
    const auto k = std::make_pair(static_cast<std::int64_t>(std::llround(p.real() / merge)), static_cast<std::int64_t>(std::llround(p.imag() / merge)));
    auto [it, fresh] = index.emplace(k, g.size());
    if (fresh) g.add(p);
    return it->second;
  };
  for (const auto& r : rings) {
    if (r.size() < 3) continue;
    std::vector<std::size_t> id;
    for (const auto& p : r) id.push_back(node(p));
    for (std::size_t k = 0; k < id.size(); k++) g.connect(id[k], id[(k + 1) % id.size()]);
  }
  return g;
}

}  // namespace architecture::syntax
