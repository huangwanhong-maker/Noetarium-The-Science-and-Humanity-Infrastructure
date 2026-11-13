// What a network's hydraulics mean for its service: how old the water is when it arrives, how much head it has to spare, and
// which pipes it cannot lose (theory note §4).
//
//   age(network, snapshot)          the age of the water at each node in a steady state: the flow-weighted mean of the ages of what
//                                   enters, each increased by its travel time L A / |Q| along its pipe (none through a pump); sources
//                                   (reservoirs, and tanks that discharge) hold water of age zero, which is a declared presumption for
//                                   tanks; a node nothing flows into has no age. EPANET reaches the same ages by transporting water
//                                   over time until they settle (Rossman and Boulos 1996, J. Water Resour. Plan. Manage. 122: 137–146)
//   resilience(network, snapshot, p) Todini's index (2000, Urban Water 2: 115–122, eq. 9): the share of the power that could be spared
//                                   at the junctions above a required pressure p that is spared,
//                                   I_r = Σ_i q_i (h_i − h_i*) / (Σ_r Q_r H_r + Σ_k Q_k ΔH_k − Σ_i q_i h_i*), h_i* = z_i + p
//                                   (reservoirs' and tanks' outflows Q_r at their heads, pumps' flows Q_k at their head rises)
//   criticality(network, options, links)   each link closed in turn under pressure-driven demand: the share of the demand still
//                                   delivered, and the junctions left below the required pressure (the "N − 1" analysis of WNTR's
//                                   criticality studies, Klise et al. 2017, Environ. Model. Softw. 95: 420–431)

#pragma once

#include <civil_engineering/water/hydraulics.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <string>
#include <vector>

namespace civil_engineering::water {

inline std::vector<double> age(const Network& net, const Snapshot& s) {
  const std::size_t J = net.junctions.size(), R = net.reservoirs.size(), N = s.nodes.size(), P = net.pipes.size();
  std::map<std::string, std::size_t> node;
  for (std::size_t v = 0; v < N; v++) node[s.nodes[v]] = v;
  struct In {
    std::size_t from;
    double flow, travel;
  };
  std::vector<std::vector<In>> into(N);
  for (std::size_t e = 0; e < s.links.size(); e++) {
    const double q = s.flow[e];
    if (!s.open[e] || std::fabs(q) < 1e-12) continue;
    const std::size_t a = node.at(e < P ? net.pipes[e].from : net.pumps[e - P].from), b = node.at(e < P ? net.pipes[e].to : net.pumps[e - P].to);
    double travel = 0;
    if (e < P) travel = net.pipes[e].length * std::numbers::pi * net.pipes[e].diameter * net.pipes[e].diameter / 4 / std::fabs(q);
    if (q > 0) into[b].push_back({a, q, travel});
    else into[a].push_back({b, -q, travel});
  }
  std::vector<double> ages(N, NAN);
  for (std::size_t v = J; v < J + R; v++) ages[v] = 0;
  for (std::size_t v = J + R; v < N; v++)
    if (s.demand[v] <= 0) ages[v] = 0;  // a tank that discharges, or holds still
  // In a steady state water runs down from the sources without returning, so sweeping until nothing changes settles in at most N sweeps.
  for (std::size_t sweep = 0; sweep <= N; sweep++) {
    bool changed = false;
    for (std::size_t v = 0; v < N; v++) {
      if (v >= J && v < J + R) continue;
      if (v >= J + R && s.demand[v] <= 0) continue;
      double w = 0, sum = 0;
      bool known = !into[v].empty();
      for (const auto& in : into[v]) {
        if (std::isnan(ages[in.from])) {
          known = false;
          break;
        }
        w += in.flow, sum += in.flow * (ages[in.from] + in.travel);
      }
      if (known && w > 0) {
        const double a = sum / w;
        if (std::isnan(ages[v]) || std::fabs(a - ages[v]) > 1e-9 * (1 + std::fabs(a))) ages[v] = a, changed = true;
      }
    }
    if (!changed) break;
  }
  return ages;
}

struct Resilience {
  double index = 0;          // Todini's I_r
  double surplus = 0;        // Σ q_i (h_i − h_i*), in m⁴/s (multiply by ρg for watts)
  double available = 0;      // the denominator
  std::size_t deficient = 0; // junctions with demand below the required pressure
};

inline Resilience resilience(const Network& net, const Snapshot& s, double required_pressure) {
  const std::size_t J = net.junctions.size(), P = net.pipes.size();
  Resilience r;
  double asked = 0, input = 0;
  for (std::size_t i = 0; i < J; i++) {
    if (!(s.demand[i] > 0)) continue;
    const double hstar = net.junctions[i].elevation + required_pressure;
    r.surplus += s.demand[i] * (s.head[i] - hstar);
    asked += s.demand[i] * hstar;
    r.deficient += s.head[i] < hstar;
  }
  for (std::size_t v = J; v < s.nodes.size(); v++)
    if (s.demand[v] < 0) input += -s.demand[v] * s.head[v];  // a source's outflow at its head
  for (std::size_t k = 0; k < net.pumps.size(); k++) input += std::max(s.flow[P + k], 0.0) * std::max(-s.headloss[P + k], 0.0);
  r.available = input - asked;
  r.index = r.available > 0 ? r.surplus / r.available : NAN;
  return r;
}

struct Critical {
  std::string link;
  double delivered = 0;          // the share of the demand still delivered with the link closed
  std::size_t below = 0;         // junctions with demand left below the required pressure
};

inline std::vector<Critical> criticality(const Network& net, Options o, const std::vector<std::string>& links) {
  o.pressure_driven = true;
  std::vector<Critical> out;
  for (const auto& id : links) {
    Options c = o;
    c.open[id] = false;
    const auto s = solve(net, c);
    double asked = 0, got = 0;
    std::size_t below = 0;
    for (std::size_t i = 0; i < net.junctions.size(); i++) {
      asked += s.required[i], got += s.demand[i];
      if (s.required[i] > 0 && !(s.pressure[i] >= o.required_pressure - 1e-6)) below++;
    }
    out.push_back({id, asked > 0 ? got / asked : 1.0, below});
  }
  std::sort(out.begin(), out.end(), [](const Critical& a, const Critical& b) { return a.delivered < b.delivered; });
  return out;
}

}  // namespace civil_engineering::water
