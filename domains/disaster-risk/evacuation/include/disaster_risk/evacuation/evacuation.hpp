// Evacuation on foot: who can reach safety in time, how fast a place could be cleared at best, how fast it is cleared by people who
// leave when they do and walk to the nearest shelter with room, and where another shelter would save most (theory/disaster-risk.md §3).
//
// THE WALKING NETWORK. The roads' graph (urban risk management's lifelines), with links a hazard has closed left out and links
// shorter than a least length merged away (their ends become one node), so that a crowd's cells are not shorter than a step can
// cross. Each edge has its length, a width people may use, and the share of walking speed it keeps.
//
// SAFETY. Sinks are shelters (designated sites, with a capacity or none stated) and, if a work says so, safe ground: nodes outside
// the hazard's reach, absorbing whoever arrives (Japan's evacuation target points at the edge of a tsunami's inundation, for example).
//
// FOUR QUESTIONS.
//
// 1. Reach. The time each place takes to walk freely to the nearest sink, and who arrives within a warning time W; with capacities,
//    the most who can be housed within W, a flow from places to the sinks each reaches within W (morphe's max_flow).
//
// 2. The bound. Flows over time on the time-expanded network (morphe's flow_over_time): each edge passes at most
//    width × capacity × step people a step and takes ⌊time/step⌋ steps; people are released at the steps their departures allow, and
//    may wait anywhere. Transit times rounded down, departures moved to the start of their step and waiting allowed make the
//    discrete network a relaxation, so that its value at every instant bounds from above how many any routing of the same people,
//    leaving no earlier, could have brought to safety by then.
//
// 3. The simulation. The crowd as a density on the edges obeying Lighthill, Whitham and Richards's conservation law with a
//    triangular flux, free speed v, capacity q_max per metre of width and jam density ρ_j per square metre (physis's kinematic waves,
//    Godunov's scheme): people enter at their nodes as their departures release them and the first cell admits them, follow the
//    tree of shortest walks to the nearest sink with room, are admitted by a sink until it is full, and are rerouted when one fills.
//    People enter through a link of one cell at their node and leave through one at their sink, which adds two cells' walk.
//    Its curve lies below the bound, up to the first-order scheme's numerical diffusion, which lets a little of a crowd's front run
//    ahead of the free speed (measured in the checks); the gap between the bound with everyone leaving at once, the bound with the
//    people's departures, and the simulation splits what departure costs from what routing and crowding cost.
//
// 4. Planning. With shelters as sinks of given capacity, the most housed within W is the rank of a polymatroid in the set of
//    shelters open (a flow to a chosen set of sinks; Fujishige 2005, Submodular Functions and Optimization, §2.3), monotone and
//    submodular, so calx's greedy choice of k candidate shelters reaches 1 − 1/e of the best with a bound from the instance.
//
// What is declared. Walking speed (1.0 m/s, and 0.5 m/s for those who walk with difficulty, in the Fire and Disaster Management
// Agency's planning manual of 2002; 0.62 m/s observed on average in the 2011 tsunami by MLIT's survey), capacity per metre of width
// (1.3 people a second), jam density (5.4 per m², Weidmann 1993), the share of a road's width people use, departures as a cumulative
// share against time, the step and the horizon. Turning, stairs, cars and bicycles, and people's choice of shelter other than the
// nearest with room are not modelled.

#pragma once

#include <urban_risk_management/lifelines/lifelines.hpp>

#include <calx/optimisation/submodular.hpp>
#include <morphe/combinatorics/flows.hpp>
#include <morphe/combinatorics/shortest_paths.hpp>
#include <physis/continuum/kinematic_waves.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace disaster_risk::evacuation {

namespace ll = urban_risk_management::lifelines;
namespace mc = morphe::combinatorics;
namespace pc = physis::continuum;
inline constexpr std::size_t none = static_cast<std::size_t>(-1);
inline constexpr double inf = std::numeric_limits<double>::infinity();

struct Walking {
  double speed = 1.0;        // m/s, free walking
  double gap_speed = 1.0;    // m/s, from a place to the network
  double capacity = 1.3;     // people per metre of width per second
  double jam = 5.4;          // people per m² at a standstill
  double width_share = 1.0;  // the share of a road's width people use
  double least_width = 1.5;  // m: no edge narrower
  void check() const {
    if (!(speed > 0 && gap_speed > 0 && capacity > 0 && jam > 0 && width_share > 0 && least_width > 0))
      throw std::invalid_argument("disaster_risk::evacuation: walking's speed, capacity, jam density and widths are > 0");
    if (!(jam * speed > capacity)) throw std::invalid_argument("disaster_risk::evacuation: the jam density times the speed exceeds the capacity");
  }
};

// Departures: the share of people who have left by each time after the warning (s), read linearly, non-decreasing to 1.
struct Departure {
  std::vector<double> time{0, 1e-9};
  std::vector<double> share{0, 1};
  std::string source = "everyone at once";
  double operator()(double t) const {
    if (t <= time.front()) return share.front();
    if (t >= time.back()) return share.back();
    const std::size_t k = static_cast<std::size_t>(std::upper_bound(time.begin(), time.end(), t) - time.begin());
    return share[k - 1] + (t - time[k - 1]) / (time[k] - time[k - 1]) * (share[k] - share[k - 1]);
  }
  void check() const {
    if (time.size() != share.size() || time.size() < 2) throw std::invalid_argument("disaster_risk::evacuation: a departure curve of at least two points");
    for (std::size_t k = 0; k < time.size(); k++) {
      if (!(share[k] >= 0 && share[k] <= 1) || (k > 0 && (!(time[k] > time[k - 1]) || share[k] < share[k - 1])))
        throw std::invalid_argument("disaster_risk::evacuation: a departure curve ascends in time, its shares non-decreasing in [0, 1]");
    }
    if (std::fabs(share.back() - 1) > 1e-12) throw std::invalid_argument("disaster_risk::evacuation: everyone has left by the curve's end");
  }
  static Departure at_once(double t0 = 0) { return {{t0, t0 + 1e-9}, {0, 1}, "everyone at once"}; }
};

struct Edge {
  std::size_t a = 0, b = 0;
  double length = 0, width = 0, factor = 1;  // m, m, the share of walking speed kept
  std::size_t link = none;                   // the graph's link
};

struct Network {
  std::vector<double> x, y;                  // nodes (m)
  std::vector<Edge> edges;
  std::vector<std::size_t> node_of;          // graph junction → node
  std::vector<std::vector<std::size_t>> incident;
  double time(std::size_t e, const Walking& w) const { return edges[e].length / (w.speed * edges[e].factor); }
};

// The walking network of a graph at a state of its links (closed ones left out), links shorter than `least` merged away.
inline Network network(const ll::Graph& g, const ll::State& phi, const Walking& w, double least = 5) {
  w.check();
  if (phi.size() != g.links.size()) throw std::invalid_argument("disaster_risk::evacuation::network: a state gives a share for each link");
  const std::size_t J = g.junctions();
  std::vector<std::size_t> up(J);
  std::iota(up.begin(), up.end(), std::size_t{0});
  const auto find = [&](std::size_t a) {
    while (up[a] != a) a = up[a] = up[up[a]];
    return a;
  };
  for (std::size_t l = 0; l < g.links.size(); l++)
    if (phi[l] > 0 && g.links[l].length < least) {
      const std::size_t a = find(g.links[l].from), b = find(g.links[l].to);
      if (a != b) up[a] = b;
    }
  Network n;
  std::vector<std::size_t> id(J, none);
  n.node_of.assign(J, none);
  std::vector<double> sx, sy, cnt;
  for (std::size_t j = 0; j < J; j++) {
    const std::size_t r = find(j);
    if (id[r] == none) {
      id[r] = sx.size();
      sx.push_back(0), sy.push_back(0), cnt.push_back(0);
    }
    n.node_of[j] = id[r];
    sx[id[r]] += g.x[g.point_of[j]], sy[id[r]] += g.y[g.point_of[j]], cnt[id[r]] += 1;
  }
  for (std::size_t k = 0; k < sx.size(); k++) n.x.push_back(sx[k] / cnt[k]), n.y.push_back(sy[k] / cnt[k]);
  n.incident.assign(n.x.size(), {});
  for (std::size_t l = 0; l < g.links.size(); l++) {
    if (!(phi[l] > 0)) continue;
    const std::size_t a = n.node_of[g.links[l].from], b = n.node_of[g.links[l].to];
    if (a == b) continue;
    n.edges.push_back({a, b, std::max(g.links[l].length, least), std::max(w.least_width, g.links[l].width * w.width_share), phi[l], l});
    n.incident[a].push_back(n.edges.size() - 1);
    n.incident[b].push_back(n.edges.size() - 1);
  }
  return n;
}

// A place on the walking network: its node (the nearer end of its link) and the time to walk to it.
struct Joined {
  std::size_t node = none;
  double walk = 0;  // s
};

inline Joined join(const ll::Graph& g, const Network& n, const ll::Attachment& a, const Walking& w) {
  Joined out;
  if (a.junction != none) {
    out.node = n.node_of[a.junction];
    out.walk = a.gap_m / w.gap_speed;
    return out;
  }
  const ll::Link& L = g.links[a.link];
  const double along = L.time > 0 ? a.from / L.time * L.length : 0;
  const bool near_from = along <= L.length - along;
  out.node = n.node_of[near_from ? L.from : L.to];
  out.walk = a.gap_m / w.gap_speed + (near_from ? along : L.length - along) / w.speed;
  return out;
}

struct People {
  std::vector<Joined> at;
  std::vector<double> count;
};
struct Sinks {
  std::vector<std::size_t> node;       // per sink
  std::vector<double> capacity;        // per sink (+inf: none stated)
  std::vector<std::string> name;
};

inline People people_on(const ll::Graph& g, const Network& n, const std::vector<ll::Place>& places, const Walking& w) {
  const ll::Locator locate(g);
  People p;
  for (const auto& pl : places) {
    if (!(pl.weight >= 0 && std::isfinite(pl.weight))) throw std::invalid_argument("disaster_risk::evacuation: people are finite and ≥ 0");
    p.at.push_back(join(g, n, ll::attach(g, locate, pl.x, pl.y, w.gap_speed), w));
    p.count.push_back(pl.weight);
  }
  return p;
}

// Shelters as sinks (a place's weight its capacity; ≤ 0 or +inf for none stated), and safe ground as sinks without capacity.
inline Sinks sinks_of(const ll::Graph& g, const Network& n, const std::vector<ll::Place>& shelters, const std::vector<char>& safe_node, const Walking& w,
                      const std::vector<std::string>& names = {}) {
  const ll::Locator locate(g);
  Sinks s;
  for (std::size_t k = 0; k < shelters.size(); k++) {
    const auto j = join(g, n, ll::attach(g, locate, shelters[k].x, shelters[k].y, w.gap_speed), w);
    s.node.push_back(j.node);
    s.capacity.push_back(shelters[k].weight > 0 ? shelters[k].weight : inf);
    s.name.push_back(k < names.size() ? names[k] : "shelter " + std::to_string(k + 1));
  }
  for (std::size_t v = 0; v < safe_node.size() && v < n.x.size(); v++)
    if (safe_node[v]) {
      s.node.push_back(v);
      s.capacity.push_back(inf);
      s.name.push_back("safe ground");
    }
  return s;
}

namespace detail {

inline mc::WeightedAdjacency adjacency(const Network& n, const Walking& w) {
  mc::WeightedAdjacency adj(n.x.size());
  for (std::size_t e = 0; e < n.edges.size(); e++) {
    const double t = n.time(e, w);
    adj[n.edges[e].a].push_back({n.edges[e].b, t});
    adj[n.edges[e].b].push_back({n.edges[e].a, t});
  }
  return adj;
}

}  // namespace detail

// ---- 1. reach ---------------------------------------------------------------------------------------------------------------------

struct Reach {
  double people = 0;
  double within = 0;                 // who can walk freely to some sink within the warning time
  double housed = 0;                 // the most who can be brought within it to sinks of their capacities
  std::vector<double> time;          // per place, s to the nearest sink (+inf: none reached)
  std::vector<double> load;          // per sink of stated capacity, in the flow that houses the most (NaN for those without one)
};

namespace detail {
// Sinks as groups for the flows of reach and planning: each sink of stated capacity a group of its own, every sink without one in a
// single group, since a flow cannot tell them apart; so that the many nodes of dry ground cost one search, not one each.
struct Groups {
  std::vector<std::vector<std::size_t>> nodes;  // per group, its sinks' nodes
  std::vector<double> capacity;                 // per group
  std::vector<std::size_t> of;                  // per sink, its group
};
inline Groups groups_of(const Sinks& s) {
  Groups g;
  std::size_t unbounded = none;
  for (std::size_t k = 0; k < s.node.size(); k++) {
    if (std::isfinite(s.capacity[k])) {
      g.of.push_back(g.nodes.size());
      g.nodes.push_back({s.node[k]});
      g.capacity.push_back(s.capacity[k]);
    } else {
      if (unbounded == none) {
        unbounded = g.nodes.size();
        g.nodes.emplace_back();
        g.capacity.push_back(inf);
      }
      g.nodes[unbounded].push_back(s.node[k]);
      g.of.push_back(unbounded);
    }
  }
  return g;
}
// Whether each place reaches each group within W: one search from all of a group's nodes, bounded by W.
inline std::vector<std::vector<char>> reaches(const Network& n, const People& p, const Groups& g, const Walking& w, double W) {
  const auto adj = adjacency(n, w);
  std::vector<std::vector<char>> R(g.nodes.size(), std::vector<char>(p.at.size(), 0));
  for (std::size_t k = 0; k < g.nodes.size(); k++) {
    std::vector<std::pair<std::size_t, double>> src;
    for (std::size_t v : g.nodes[k]) src.push_back({v, 0});
    const auto P = mc::dijkstra(adj, src, W);
    for (std::size_t i = 0; i < p.at.size(); i++) R[k][i] = p.at[i].node != none && p.at[i].walk + P.distance[p.at[i].node] <= W;
  }
  return R;
}
inline double housed(const People& p, const std::vector<double>& capacity, const std::vector<std::vector<char>>& R, const std::vector<std::size_t>& open,
                     std::vector<double>* load = nullptr) {
  const std::size_t P = p.at.size(), K = open.size(), S = P + K, T = S + 1;
  std::vector<mc::FlowArc> arcs;
  for (std::size_t i = 0; i < P; i++)
    if (p.count[i] > 0) arcs.push_back({S, i, p.count[i]});
  for (std::size_t k = 0; k < K; k++) {
    for (std::size_t i = 0; i < P; i++)
      if (R[open[k]][i] && p.count[i] > 0) arcs.push_back({i, P + k, inf});
    arcs.push_back({P + k, T, capacity[open[k]]});
  }
  if (arcs.empty()) return 0;
  const auto M = mc::max_flow(T + 1, arcs, S, T);
  if (load) {
    load->assign(capacity.size(), 0);
    for (std::size_t a = 0; a < arcs.size(); a++)
      if (arcs[a].to == T) (*load)[open[arcs[a].from - P]] = M.flow[a];
  }
  return M.value;
}
}  // namespace detail

inline Reach reach(const Network& n, const People& p, const Sinks& s, const Walking& w, double W) {
  w.check();
  if (s.node.empty()) throw std::invalid_argument("disaster_risk::evacuation::reach: at least one sink");
  Reach out;
  const auto adj = detail::adjacency(n, w);
  std::vector<std::pair<std::size_t, double>> src;
  for (std::size_t v : s.node) src.push_back({v, 0});
  const auto D = mc::dijkstra(adj, src);
  for (std::size_t i = 0; i < p.at.size(); i++) {
    const double t = p.at[i].node == none ? inf : p.at[i].walk + D.distance[p.at[i].node];
    out.time.push_back(t);
    out.people += p.count[i];
    if (t <= W) out.within += p.count[i];
  }
  const auto G = detail::groups_of(s);
  const auto R = detail::reaches(n, p, G, w, W);
  std::vector<std::size_t> all(G.nodes.size());
  std::iota(all.begin(), all.end(), std::size_t{0});
  std::vector<double> load;
  out.housed = detail::housed(p, G.capacity, R, all, &load);
  // A sink of stated capacity carries its group's load; those without one share a group, and their loads are not told apart.
  for (std::size_t k = 0; k < s.node.size(); k++) out.load.push_back(std::isfinite(s.capacity[k]) ? load[G.of[k]] : std::numeric_limits<double>::quiet_NaN());
  return out;
}

// ---- 2. the bound -----------------------------------------------------------------------------------------------------------------

struct Bound {
  double step = 0;                 // s
  double total = 0;                // people
  std::vector<double> safe;        // safe[k]: the most that can be safe by k steps (an upper bound for the continuous problem)
  double clearance = inf;          // s: the least time by which all can be safe, +inf if not within the horizon
  double at(double t) const {      // the bound at an instant (s): the step's value at ⌈t/step⌉, which is still an upper bound
    if (safe.empty()) return 0;
    const auto k = static_cast<std::size_t>(std::ceil(t / step - 1e-9));
    return safe[std::min(k, safe.size() - 1)];
  }
};

inline Bound bound(const Network& n, const People& p, const Sinks& s, const Walking& w, const Departure& dep, double step, double horizon) {
  w.check();
  dep.check();
  if (!(step > 0 && horizon > 0)) throw std::invalid_argument("disaster_risk::evacuation::bound: a step and a horizon > 0");
  if (s.node.empty()) throw std::invalid_argument("disaster_risk::evacuation::bound: at least one sink");
  const auto H = static_cast<std::size_t>(std::ceil(horizon / step));
  std::vector<mc::TimedArc> arcs;
  for (std::size_t e = 0; e < n.edges.size(); e++) {
    const auto tau = static_cast<std::size_t>(std::floor(n.time(e, w) / step));
    const double cap = n.edges[e].width * w.capacity * step;
    arcs.push_back({n.edges[e].a, n.edges[e].b, tau, cap});
    arcs.push_back({n.edges[e].b, n.edges[e].a, tau, cap});
  }
  // People by node, each node's walk the least of its places' (so that the bound also holds for the simulation, which starts a
  // node's people after their mean walk).
  std::map<std::size_t, std::pair<double, double>> by_node;  // node → (people, least walk)
  for (std::size_t i = 0; i < p.at.size(); i++) {
    if (p.at[i].node == none || !(p.count[i] > 0)) continue;
    auto it = by_node.find(p.at[i].node);
    if (it == by_node.end()) by_node[p.at[i].node] = {p.count[i], p.at[i].walk};
    else it->second.first += p.count[i], it->second.second = std::min(it->second.second, p.at[i].walk);
  }
  std::map<std::pair<std::size_t, std::size_t>, double> released;  // (node, step) → people
  Bound out;
  out.step = step;
  for (const auto& [v, pw] : by_node) {
    const auto [count, walk] = pw;
    out.total += count;
    double before = 0;
    for (std::size_t k = 0; k <= H; k++) {
      const double now = dep((k + 1) * step);
      const double mass = count * (now - before);
      before = now;
      if (mass <= 0) continue;
      const auto at = static_cast<std::size_t>(std::floor((k * step + walk) / step));
      if (at <= H) released[{v, at}] += mass;
    }
  }
  std::vector<mc::TimedSupply> supplies;
  for (const auto& [key, m] : released) supplies.push_back({key.first, m, key.second});
  std::map<std::size_t, double> cap;
  for (std::size_t k = 0; k < s.node.size(); k++) cap[s.node[k]] += s.capacity[k];
  std::vector<mc::TimedSink> sinks;
  for (const auto& [v, c] : cap) sinks.push_back({v, c});
  const auto O = mc::flow_over_time(n.x.size(), arcs, supplies, sinks, H);
  out.safe = O.safe;
  if (O.quickest != mc::OverTime::none && out.total > 0 && O.safe[O.quickest] >= out.total * (1 - 1e-9)) out.clearance = O.quickest * step;
  return out;
}

// ---- 3. the simulation ------------------------------------------------------------------------------------------------------------

struct Simulation {
  double step = 0;                       // s, the scheme's
  double total = 0;                      // people released into the network
  std::vector<double> times;             // s, at each reading
  std::vector<double> safe;              // people safe by each reading
  std::vector<double> departed;          // people who have left by each reading (released by their departures)
  std::vector<double> arrived;           // per sink, people admitted
  std::vector<double> full_at;           // per sink, s when it filled (+inf: never)
  std::vector<double> peak;              // per edge, the greatest density in any of its cells (people per m²)
  std::vector<double> crowded;           // per edge, person-seconds spent in cells above 2 per m² (Fruin's level of service F)
  std::vector<double> frame_times;       // s
  std::vector<std::vector<float>> frames;  // per frame, per edge: people per m² on average over both ways
  double cleared = 0.99;                 // the share whose safety marks clearance
  double clearance = inf;                // s, when that share was safe (+inf: not within the horizon)
  double all_safe = inf;                 // s, when all but a millionth were safe: the first-order scheme smears a crowd's rear, so this
                                         // runs later than clearance by more than the people's own spread
  std::size_t reroutes = 0;              // how often a sink filled and the routes changed
  double residual = 0;                   // people released − safe − on the network − waiting to enter: zero to rounding
};

inline Simulation simulate(const Network& n, const People& p, const Sinks& s, const Walking& w, const Departure& dep, double horizon,
                           double read_every = 30, double frame_every = 60, double dx = 10, double cleared = 0.99) {
  w.check();
  dep.check();
  if (s.node.empty()) throw std::invalid_argument("disaster_risk::evacuation::simulate: at least one sink");
  if (!(horizon > 0 && read_every > 0 && frame_every > 0 && dx > 0)) throw std::invalid_argument("disaster_risk::evacuation::simulate: a horizon, readings, frames and a cell length > 0");
  if (!(cleared > 0 && cleared <= 1)) throw std::invalid_argument("disaster_risk::evacuation::simulate: a share cleared in (0, 1]");
  const std::size_t N = n.x.size();
  // Nodes in parts of the network that hold a sink; people elsewhere cannot be routed and are counted out.
  std::vector<std::size_t> part(N);
  std::iota(part.begin(), part.end(), std::size_t{0});
  const auto find = [&](std::size_t a) {
    while (part[a] != a) a = part[a] = part[part[a]];
    return a;
  };
  for (const auto& e : n.edges) part[find(e.a)] = find(e.b);
  std::vector<char> live(N, 0);
  for (std::size_t v : s.node) live[find(v)] = 1;
  std::vector<char> active(N, 0);
  for (std::size_t v = 0; v < N; v++) active[v] = live[find(v)] && !n.incident[v].empty();
  // People by node.
  std::vector<double> pop(N, 0), delay(N, 0), weight(N, 0);
  for (std::size_t i = 0; i < p.at.size(); i++) {
    const std::size_t v = p.at[i].node;
    if (v == none || !active[v]) continue;
    pop[v] += p.count[i];
    weight[v] += p.count[i];
    delay[v] += p.count[i] * p.at[i].walk;
  }
  for (std::size_t v = 0; v < N; v++)
    if (weight[v] > 0) delay[v] /= weight[v];  // the people of a node start walking it after a mean delay of their gaps
  // Sinks by node, their capacities summed.
  std::vector<std::size_t> sink_at(N, none);
  std::vector<std::size_t> sink_nodes;
  std::vector<double> sink_cap;
  for (std::size_t k = 0; k < s.node.size(); k++) {
    const std::size_t v = s.node[k];
    if (!active[v]) continue;
    if (sink_at[v] == none) {
      sink_at[v] = sink_nodes.size();
      sink_nodes.push_back(v);
      sink_cap.push_back(0);
    }
    sink_cap[sink_at[v]] += s.capacity[k];
  }
  // The links of the scheme: two per edge, then an entry per node with people, then an exit per sink node.
  const auto flux_for = [&](double width, double factor) {
    const double v = w.speed * factor, q = w.capacity * width, rj = w.jam * width;
    const double rc = q / v;
    return pc::Flux::triangular(v, q / (rj - rc), rj);
  };
  std::vector<pc::Link> links;
  std::vector<std::size_t> link_edge;
  for (std::size_t e = 0; e < n.edges.size(); e++) {
    const auto& E = n.edges[e];
    if (!active[E.a]) continue;
    const auto cells = static_cast<std::size_t>(std::max(1.0, std::round(E.length / dx)));
    links.push_back({E.a, E.b, E.length, cells, flux_for(E.width, E.factor)});
    link_edge.push_back(e);
    links.push_back({E.b, E.a, E.length, cells, flux_for(E.width, E.factor)});
    link_edge.push_back(e);
  }
  const std::size_t edge_links = links.size();
  std::size_t next_node = N;
  std::vector<std::size_t> entry_link(N, none), exit_link(sink_nodes.size(), none), exit_node(sink_nodes.size(), none);
  const double wide = 20;  // m: entries and exits do not hold people back
  for (std::size_t v = 0; v < N; v++)
    if (pop[v] > 0) {
      links.push_back({next_node, v, dx, 1, flux_for(wide, 1)});
      entry_link[v] = links.size() - 1;
      next_node++;
    }
  for (std::size_t k = 0; k < sink_nodes.size(); k++) {
    links.push_back({sink_nodes[k], next_node, dx, 1, flux_for(wide, 1)});
    exit_link[k] = links.size() - 1;
    exit_node[k] = next_node++;
  }
  pc::KinematicNetwork net(next_node, links);
  const double h = 0.95 * net.cfl();
  Simulation out;
  out.step = h;
  out.cleared = cleared;
  std::vector<double> admitted(N, 0), remaining(sink_cap);
  std::vector<char> full(sink_nodes.size(), 0);
  out.arrived.assign(sink_nodes.size(), 0);
  out.full_at.assign(sink_nodes.size(), inf);
  for (std::size_t v = 0; v < N; v++)
    if (entry_link[v] != none) {
      const double* adm = &admitted[v];
      const double P = pop[v], d = delay[v];
      net.entry(links[entry_link[v]].tail, [P, d, adm, h, &dep](double t) { return std::max(0.0, P * dep(t - d) - *adm) / h; });
    }
  for (std::size_t k = 0; k < sink_nodes.size(); k++) {
    const double* rem = &remaining[k];
    net.exit(exit_node[k], [rem, h](double) { return std::isfinite(*rem) ? std::max(0.0, *rem) / h : 1e18; });
  }
  // Routes: the tree of shortest walks to the nearest sink with room (or to any sink when none has room).
  const auto adj = detail::adjacency(n, w);
  const auto route = [&]() {
    std::vector<std::pair<std::size_t, double>> src;
    for (std::size_t k = 0; k < sink_nodes.size(); k++)
      if (!full[k]) src.push_back({sink_nodes[k], 0});
    if (src.empty())
      for (std::size_t k = 0; k < sink_nodes.size(); k++) src.push_back({sink_nodes[k], 0});
    const auto P = mc::dijkstra(adj, src);
    for (std::size_t v = 0; v < N; v++) {
      if (!active[v]) continue;
      const auto& outs = net.outgoing(v);
      const auto& ins = net.incoming(v);
      if (outs.empty() || ins.empty()) continue;
      std::size_t target = none;
      if (sink_at[v] != none && (!full[sink_at[v]] || P.parent[v] == mc::ShortestPaths::none)) target = exit_link[sink_at[v]];
      else if (P.parent[v] != mc::ShortestPaths::none) {
        const std::size_t u = P.parent[v];
        double best = inf;
        for (std::size_t l : outs)
          if (l < edge_links && links[l].head == u && n.time(link_edge[l], w) < best) best = n.time(link_edge[l], w), target = l;
      }
      if (target == none) target = outs.front();  // unreachable within its part: should not happen in a live part
      std::vector<std::vector<double>> beta(ins.size(), std::vector<double>(outs.size(), 0));
      const std::size_t col = static_cast<std::size_t>(std::find(outs.begin(), outs.end(), target) - outs.begin());
      for (auto& row : beta) row[col] = 1;
      net.junction(v, beta, pc::KinematicNetwork::Rule::non_fifo);
    }
  };
  route();
  for (std::size_t v = 0; v < N; v++) out.total += pop[v];
  out.peak.assign(n.edges.size(), 0);
  out.crowded.assign(n.edges.size(), 0);
  auto state = net.start(0);
  double safe = 0, next_read = 0, next_frame = 0;
  const auto read = [&](double t) {
    double departed = 0;
    for (std::size_t v = 0; v < N; v++)
      if (pop[v] > 0) departed += pop[v] * dep(t - delay[v]);
    out.times.push_back(t);
    out.safe.push_back(safe);
    out.departed.push_back(departed);
  };
  const auto frame = [&](double t) {
    std::vector<float> f(n.edges.size(), 0);
    for (std::size_t l = 0; l < edge_links; l++) {
      double c = 0;
      for (double r : state.density[l]) c += r * net.dx(l);
      const auto& E = n.edges[link_edge[l]];
      f[link_edge[l]] += static_cast<float>(c / (E.length * E.width));
    }
    out.frame_times.push_back(t);
    out.frames.push_back(std::move(f));
  };
  double t = 0;
  while (t < horizon - 1e-9) {
    if (t >= next_read - 1e-9) read(t), next_read += read_every;
    if (t >= next_frame - 1e-9) frame(t), next_frame += frame_every;
    // The flows of this step, read before it is taken: the entries' demands and the exits' supplies depend on what has been admitted
    // and what room is left, which change only after the step, so that the step moves exactly these flows.
    const auto F = net.link_flows(state);
    for (std::size_t l = 0; l < edge_links; l++) {
      const auto& E = n.edges[link_edge[l]];
      for (double r : state.density[l]) {
        const double per_m2 = r / E.width;
        out.peak[link_edge[l]] = std::max(out.peak[link_edge[l]], per_m2);
        if (per_m2 > 2.0) out.crowded[link_edge[l]] += h * r * net.dx(l);
      }
    }
    state = net.step(state, h);
    for (std::size_t v = 0; v < N; v++)
      if (entry_link[v] != none) admitted[v] += h * F[entry_link[v]][0];
    bool filled = false;
    for (std::size_t k = 0; k < sink_nodes.size(); k++) {
      const double in = h * F[exit_link[k]].back();
      out.arrived[k] += in;
      safe += in;
      if (std::isfinite(remaining[k])) {
        remaining[k] -= in;
        if (!full[k] && remaining[k] <= 1e-9 * std::max(1.0, sink_cap[k])) full[k] = 1, out.full_at[k] = t + h, filled = true;
      }
    }
    t += h;
    if (filled) route(), out.reroutes++;
    if (!std::isfinite(out.clearance) && out.total > 0 && safe >= out.total * cleared) out.clearance = t;
    if (!std::isfinite(out.all_safe) && out.total > 0 && safe >= out.total * (1 - 1e-6)) out.all_safe = t;
    if (std::isfinite(out.all_safe) && t >= out.all_safe + read_every) break;
  }
  read(t);
  frame(t);
  double released = 0;
  for (std::size_t v = 0; v < N; v++) released += admitted[v];
  out.residual = released - safe - net.content(state);
  return out;
}

// ---- 4. planning ------------------------------------------------------------------------------------------------------------------

struct Plan {
  std::vector<std::size_t> chosen;   // candidate shelters in the order chosen
  std::vector<double> value;         // the most housed within W after each choice
  double existing = 0;               // with the existing sinks alone
  double bound = 0;                  // the best k candidates house at most this
  double guarantee = 0;              // 1 − 1/e
  double total() const { return value.empty() ? existing : value.back(); }
};

// The k candidates (sinks of their capacities) that, added to the existing sinks, house most within the warning time W.
inline Plan plan(const Network& n, const People& p, const Sinks& existing, const Sinks& candidates, const Walking& w, double W, std::size_t k) {
  w.check();
  if (candidates.node.empty() || k == 0) throw std::invalid_argument("disaster_risk::evacuation::plan: candidates and k ≥ 1");
  auto G = detail::groups_of(existing);
  const std::size_t E = G.nodes.size(), C = candidates.node.size();
  for (std::size_t c = 0; c < C; c++) {  // each candidate a group of its own
    G.nodes.push_back({candidates.node[c]});
    G.capacity.push_back(candidates.capacity[c]);
  }
  const auto R = detail::reaches(n, p, G, w, W);
  const std::function<double(const std::vector<std::size_t>&)> f = [&](const std::vector<std::size_t>& set) {
    std::vector<std::size_t> open(E);
    std::iota(open.begin(), open.end(), std::size_t{0});
    for (std::size_t c : set) open.push_back(E + c);
    return detail::housed(p, G.capacity, R, open);
  };
  const auto best = calx::optimisation::greedy_submodular(C, f, std::min(k, C));
  Plan out;
  out.chosen = best.chosen;
  out.value = best.value;
  out.existing = best.empty;
  out.bound = best.bound;
  out.guarantee = best.guarantee;
  return out;
}

// The most housed within W with exactly the sinks given open (for checks and for comparing plans).
inline double housed(const Network& n, const People& p, const Sinks& s, const Walking& w, double W) {
  const auto G = detail::groups_of(s);
  const auto R = detail::reaches(n, p, G, w, W);
  std::vector<std::size_t> open(G.nodes.size());
  std::iota(open.begin(), open.end(), std::size_t{0});
  return detail::housed(p, G.capacity, R, open);
}

}  // namespace disaster_risk::evacuation
