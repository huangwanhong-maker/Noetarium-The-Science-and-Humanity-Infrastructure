// The syntax of streets: a city's street network as a geometric graph, and its blocks as a 2-complex, for syntaxis's grammars
// (ADR 0083). The grammars themselves (learning, parsing, sampling) are syntaxis's; what is the city's is here.
//
// - street_graph: ways (polylines in a local frame, metres) snapped into a network by urban-science's access (points within `snap`
//   of one another are one), then each chain of points of degree two contracted into one edge between its ends where it runs
//   straight: a point is kept as a node where the way turns by more than `bend` degrees, so that a curve keeps its corners and a
//   straight run of many points becomes one segment. The nodes are junctions, dead ends and bends.
// - blocks: the street graph's bounded faces (physis's plane faces), triangulated, those of at most `max_area` square metres: the
//   street fabric as a 2-complex, whose triangles syntaxis reads as words.
//
// Limits: ways that cross without sharing a point (a bridge over a road) cross in the graph, and the plane faces assume none do; the
// faces about them are then not the city's blocks. Curved ways are approximated by their bends.

#pragma once

#include <urban/access/access.hpp>

#include <syntaxis/graphs/graphs.hpp>
#include <syntaxis/simplicial/simplicial.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <set>
#include <vector>

namespace urban::syntax {

using syntaxis::graphs::Graph;
using syntaxis::shape::Point;

inline Graph street_graph(const std::vector<std::vector<double>>& ways, double snap = 1.0, double bend = 20) {
  std::vector<access::Way> w;
  w.reserve(ways.size());
  for (const auto& xy : ways) w.push_back({xy, 1.0});
  const access::Network n = access::network_of(w, snap);
  const std::size_t N = n.x.size();
  std::vector<std::set<std::size_t>> adj(N);
  for (std::size_t u = 0; u < N; u++)
    for (const auto& arc : n.arcs[u]) if (arc.to != u) adj[u].insert(arc.to), adj[arc.to].insert(u);
  const auto at = [&](std::size_t v) { return Point{n.x[v], n.y[v]}; };
  // A node is kept where the degree is not two, or where the way turns by more than `bend`.
  std::vector<char> keep(N, 0);
  for (std::size_t v = 0; v < N; v++) {
    if (adj[v].empty()) continue;
    if (adj[v].size() != 2) { keep[v] = 1; continue; }
    const std::size_t a = *adj[v].begin(), b = *std::next(adj[v].begin());
    const double turn = std::abs(std::arg((at(b) - at(v)) / (at(v) - at(a)))) * 180 / std::numbers::pi;
    keep[v] = turn > bend;
  }
  // A ring of degree-two points with no kept node (a roundabout drawn alone) keeps its first point to stand for it.
  {
    std::vector<char> seen(N, 0);
    for (std::size_t v = 0; v < N; v++) {
      if (keep[v] || seen[v] || adj[v].empty()) continue;
      std::vector<std::size_t> stack{v}, part;
      bool touches = false;
      seen[v] = 1;
      while (!stack.empty()) {
        const std::size_t u = stack.back();
        stack.pop_back();
        part.push_back(u);
        for (std::size_t q : adj[u]) {
          if (keep[q]) { touches = true; continue; }
          if (!seen[q]) seen[q] = 1, stack.push_back(q);
        }
      }
      if (!touches) keep[part.front()] = 1;
    }
  }
  Graph g;
  std::vector<std::size_t> id(N, std::numeric_limits<std::size_t>::max());
  for (std::size_t v = 0; v < N; v++) if (keep[v]) id[v] = g.add(at(v));
  // Walk from each kept node along each edge through unkept nodes to the next kept one.
  std::vector<std::set<std::size_t>> walked(N);
  for (std::size_t s = 0; s < N; s++) {
    if (!keep[s]) continue;
    for (std::size_t first : adj[s]) {
      if (walked[s].count(first)) continue;
      std::size_t prev = s, cur = first;
      while (!keep[cur]) {
        const std::size_t next = *adj[cur].begin() == prev ? *std::next(adj[cur].begin()) : *adj[cur].begin();
        prev = cur, cur = next;
        if (cur == s) break;  // a loop with no kept node but s
      }
      walked[s].insert(first);
      walked[cur].insert(prev);
      if (cur != s) g.connect(id[s], id[cur]);
    }
  }
  return g;
}

inline syntaxis::simplicial::Complex blocks(const Graph& streets, double max_area = 1e6) { return syntaxis::simplicial::from_faces(streets, max_area); }

}  // namespace urban::syntax
