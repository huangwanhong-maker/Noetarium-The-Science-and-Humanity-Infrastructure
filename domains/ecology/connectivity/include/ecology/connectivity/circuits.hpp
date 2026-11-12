// Movement across a landscape of resistances: the least-cost distance between habitat patches, and circuit theory's
// effective resistance and current (McRae 2006, Evolution 60: 1551–1561; McRae et al. 2008, Ecology 89: 2712–2724).
//
// A landscape is a grid of cells, each with a resistance r > 0 to movement (+∞ a barrier). Neighbouring cells, four or eight
// about each, are joined by an edge. Two readings of the same grid:
//
//   least cost    an edge costs the cell side times the mean resistance of its two cells (times √2 on a diagonal); the
//                 least-cost distance between two patches is a shortest path (morphe's Dijkstra), the route of one
//                 walker who knows the landscape;
//   circuit       an edge conducts 1 / (mean resistance) (divided by √2 on a diagonal), Circuitscape's default for a
//                 resistance grid (McRae et al. 2008, Appendix); the effective resistance between two patches is the
//                 voltage between them when a unit current enters at one and leaves at the other, with each patch's cells
//                 joined as one node (a short circuit). It falls as routes multiply, which a least-cost distance does not
//                 see, and equals the commute time of a random walker up to a constant (Chandra et al. 1997, Comput.
//                 Complex. 6: 312–340). The current through a cell, summed over the pairs of patches, shows where
//                 movement is funnelled: pinch points.
//
// The circuit is physis's Kirchhoff network (discretisation/kirchhoff.hpp), solved by its Jacobi-preconditioned conjugate gradients
// to a relative residual of 1e-10 (a landscape's merged patches and unequal resistances sit at a rounding floor above physis's
// default; effective resistances are then good to about ten digits, far beyond what a resistance surface is known to). A cell's
// current is half the sum of the absolute flows on its edges (what enters equals what leaves), and at a patch the unit
// injected (Circuitscape's convention). Only the cells connected to the grounded patch are kept in each solve; a patch
// cut off from another by barriers is at infinite resistance from it.
#pragma once

#include <morphe/combinatorics/shortest_paths.hpp>
#include <physis/discretisation/kirchhoff.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ecology::connectivity {

struct ResistanceGrid {
  std::size_t nx = 0, ny = 0;
  double cell = 1;                  // side, metres
  std::vector<double> resistance;   // row-major, j * nx + i; +∞ a barrier
  bool eight = true;                // eight neighbours, or four
  bool passable(std::size_t c) const { return std::isfinite(resistance[c]) && resistance[c] > 0; }
};

struct CircuitResult {
  std::vector<std::vector<double>> resistance;  // effective resistance between patches (+∞ when cut off)
  std::vector<std::vector<double>> least_cost;  // least-cost distance between patches (+∞ when cut off)
  std::vector<double> current;                  // cumulative current through each cell, over every pair
};

namespace detail {

struct Edge {
  std::size_t a, b;
  double mean_r, length;  // the mean resistance of the two cells, and 1 or √2
};

inline std::vector<Edge> edges(const ResistanceGrid& g) {
  std::vector<Edge> out;
  const auto at = [&](std::size_t i, std::size_t j) { return j * g.nx + i; };
  for (std::size_t j = 0; j < g.ny; j++)
    for (std::size_t i = 0; i < g.nx; i++) {
      const std::size_t c = at(i, j);
      if (!g.passable(c)) continue;
      const auto join = [&](std::size_t i2, std::size_t j2, double len) {
        const std::size_t d = at(i2, j2);
        if (g.passable(d)) out.push_back({c, d, (g.resistance[c] + g.resistance[d]) / 2, len});
      };
      if (i + 1 < g.nx) join(i + 1, j, 1);
      if (j + 1 < g.ny) join(i, j + 1, 1);
      if (g.eight) {
        if (i + 1 < g.nx && j + 1 < g.ny) join(i + 1, j + 1, std::numbers::sqrt2);
        if (i > 0 && j + 1 < g.ny) join(i - 1, j + 1, std::numbers::sqrt2);
      }
    }
  return out;
}

}  // namespace detail

// Every pair of patches (each a list of cells): effective resistances, least-cost distances, and the cumulative current.
inline CircuitResult circuits(const ResistanceGrid& g, const std::vector<std::vector<std::size_t>>& patches) {
  const std::size_t N = g.nx * g.ny, P = patches.size();
  if (g.resistance.size() != N) throw std::invalid_argument("ecology::connectivity::circuits: one resistance per cell");
  // Each cell's node: the patch it belongs to (nodes 0..P−1) or a node of its own.
  std::vector<std::size_t> node(N, std::numeric_limits<std::size_t>::max());
  for (std::size_t p = 0; p < P; p++)
    for (std::size_t c : patches[p]) {
      if (c >= N || !g.passable(c)) throw std::invalid_argument("ecology::connectivity::circuits: a patch's cells lie on the grid and are passable");
      if (node[c] != std::numeric_limits<std::size_t>::max()) throw std::invalid_argument("ecology::connectivity::circuits: patches do not share cells");
      node[c] = p;
    }
  std::size_t V = P;
  for (std::size_t c = 0; c < N; c++)
    if (g.passable(c) && node[c] == std::numeric_limits<std::size_t>::max()) node[c] = V++;
  const auto es = detail::edges(g);

  CircuitResult out;
  out.resistance.assign(P, std::vector<double>(P, 0.0));
  out.least_cost.assign(P, std::vector<double>(P, 0.0));
  out.current.assign(N, 0.0);

  // Least cost: Dijkstra over cells from every cell of a patch at once.
  {
    morphe::combinatorics::WeightedAdjacency adj(N);
    for (const auto& e : es) {
      const double w = g.cell * e.mean_r * e.length;
      adj[e.a].push_back({e.b, w});
      adj[e.b].push_back({e.a, w});
    }
    for (std::size_t p = 0; p < P; p++) {
      std::vector<std::pair<std::size_t, double>> src;
      for (std::size_t c : patches[p]) src.emplace_back(c, 0.0);
      const auto sp = morphe::combinatorics::dijkstra(adj, src);
      for (std::size_t q = 0; q < P; q++) {
        double best = std::numeric_limits<double>::infinity();
        for (std::size_t c : patches[q]) best = std::min(best, sp.distance[c]);
        out.least_cost[p][q] = p == q ? 0.0 : best;
      }
    }
  }

  // The circuit on nodes (patch cells merged); edges inside a patch vanish.
  std::vector<std::size_t> ea, eb;
  std::vector<double> conductance;
  std::vector<std::vector<std::size_t>> incident(V);
  for (const auto& e : es) {
    const std::size_t a = node[e.a], b = node[e.b];
    if (a == b) continue;
    ea.push_back(a), eb.push_back(b), conductance.push_back(1.0 / (e.mean_r * e.length));
    incident[a].push_back(ea.size() - 1), incident[b].push_back(ea.size() - 1);
  }
  // Components of the node graph, so that each solve keeps only what touches its ground.
  std::vector<std::size_t> comp(V, std::numeric_limits<std::size_t>::max());
  for (std::size_t s = 0; s < V; s++) {
    if (comp[s] != std::numeric_limits<std::size_t>::max()) continue;
    std::vector<std::size_t> stack{s};
    comp[s] = s;
    while (!stack.empty()) {
      const std::size_t u = stack.back();
      stack.pop_back();
      for (std::size_t k : incident[u]) {
        const std::size_t v = ea[k] == u ? eb[k] : ea[k];
        if (comp[v] == std::numeric_limits<std::size_t>::max()) comp[v] = s, stack.push_back(v);
      }
    }
  }
  std::vector<double> node_current(V, 0.0);
  for (std::size_t p = 0; p < P; p++)
    for (std::size_t q = p + 1; q < P; q++) {
      if (comp[p] != comp[q]) {
        out.resistance[p][q] = out.resistance[q][p] = std::numeric_limits<double>::infinity();
        continue;
      }
      // The component's nodes, renumbered.
      std::vector<std::size_t> local(V, std::numeric_limits<std::size_t>::max());
      std::size_t n = 0;
      for (std::size_t v = 0; v < V; v++)
        if (comp[v] == comp[q]) local[v] = n++;
      physis::discretisation::ConductanceNetwork net;
      net.vertices = n;
      std::vector<std::size_t> which;
      for (std::size_t k = 0; k < ea.size(); k++)
        if (local[ea[k]] != std::numeric_limits<std::size_t>::max()) net.add(local[ea[k]], local[eb[k]], conductance[k]), which.push_back(k);
      std::vector<double> inject(n, 0.0);
      inject[local[p]] = 1;
      const auto sol = physis::discretisation::solve_kirchhoff(net, {{local[q], 0.0}}, inject, 1e-10);
      out.resistance[p][q] = out.resistance[q][p] = sol.potential[local[p]];
      std::vector<double> through(V, 0.0);
      for (std::size_t m = 0; m < which.size(); m++) {
        const double f = std::fabs(sol.flow[m]);
        through[ea[which[m]]] += f / 2, through[eb[which[m]]] += f / 2;
      }
      through[p] = through[q] = 1;  // the unit current enters at one patch and leaves at the other
      for (std::size_t v = 0; v < V; v++) node_current[v] += through[v];
    }
  for (std::size_t c = 0; c < N; c++)
    if (g.passable(c)) out.current[c] = node_current[node[c]];
  return out;
}

}  // namespace ecology::connectivity
