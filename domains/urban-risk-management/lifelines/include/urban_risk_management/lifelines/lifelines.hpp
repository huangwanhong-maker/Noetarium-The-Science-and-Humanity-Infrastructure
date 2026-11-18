// Lifelines: a city's roads as the links of a system whose service is reaching what people need, and which links matter most to it
// (theory/lifelines-and-response.md).
//
// THE GRAPH. Roads are polylines in a local frame (metres) with a free speed and a width. They are joined into a network as urban
// science's `access` joins them (points that coincide within `snap`; loose ends stitched to the nearest point and to the nearest
// segment, as tiles cut and simplify ways), and the network is contracted: junctions are the points of degree other than two, and a
// LINK is the chain of pieces between two junctions. Links are the components of the system: a link keeps a share φ ∈ [0, 1] of its
// speed (1 free, 0 closed), and its time is t/φ.
//
// PLACES AND SOURCES. People (at their homes, in cells of a population grid) and services (hospitals, fire stations, shelters) join the
// network at the nearest point a place may join (one in a connected part holding a share of the network, `access::joinable`), across a
// gap at a declared speed. A place on a link's interior reaches its two ends along it; on a closed link it reaches nothing.
//
// THE SERVICE AND ITS LOSS. The time from a place to the nearest service is a shortest path from all services at once (morphe's
// Dijkstra with offsets). The loss of a state against the intact network is
//
//     L(φ) = Σ_p people_p · (min(t_p(φ), cap) − min(t_p(1), cap)) / 60     person-minutes a trip,
//
// a place that reaches nothing counted at the cap; beside it, the people beyond a threshold. The cap is a declaration: what being cut
// off is counted as.
//
// WHAT A LINK'S FAILURE CHANGES. Closing a link changes no time unless the link carries a shortest path: a junction's time can only
// rise when a link is removed, and is unchanged if its shortest path avoids the link. So, at a state, only the links of the forest of
// shortest paths (and links on which places or services sit) are asked, and for each only the junctions beneath it in the forest are
// recomputed, from the times of their neighbours outside it (Proposition 2.2 of the theory). The criticality of every link at a state
// costs the sum of its subtrees, not one shortest-path search a link.
//
// REDUNDANCY. How many links must fail to cut an area from every service: the greatest number of link-disjoint paths from the area's
// junctions to the services' (Menger), a unit flow through each open link (morphe's max_flow), and the links of the least cut.

#pragma once

#include <risk_management/criticality/criticality.hpp>
#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>

#include <morphe/combinatorics/flows.hpp>
#include <morphe/combinatorics/shortest_paths.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace urban_risk_management::lifelines {

namespace mc = morphe::combinatorics;
namespace ua = urban::access;
namespace rc = risk_management::criticality;
inline constexpr std::size_t none = static_cast<std::size_t>(-1);
inline constexpr double inf = std::numeric_limits<double>::infinity();

struct Road {
  std::vector<double> xy;  // x0, y0, x1, y1, ... (m)
  double speed = 8.33;     // m/s at free flow
  double width = 6;        // m
  std::string kind;        // its class, carried for reading
};

struct GraphOptions {
  double snap = 1.0;          // points within this coincide (m)
  double stitch = 6.0;        // loose ends joined within this (m); 0 joins none
  double join_speed = 1.34;   // across a stitch and the gap from a place to the network (m/s)
  double join_width = 3.0;    // the width of a stitch (m)
  double share = 0.01;        // a place joins only parts of the network holding this share of its points
};

struct Link {
  std::size_t from = 0, to = 0;     // junctions
  std::vector<std::size_t> points;  // from `from`'s point to `to`'s point
  double length = 0, time = 0;      // m; s at free speed
  double width = 0;                 // the least width along it (m)
  std::size_t road = none;          // the road of most of its length; none for a stitch
};

struct Graph {
  std::vector<double> x, y;                         // points
  std::vector<std::size_t> point_of;                // junction → point
  std::vector<std::size_t> junction_at;             // point → junction, or none
  std::vector<Link> links;
  std::vector<std::vector<std::size_t>> incident;   // junction → its links
  std::vector<std::size_t> link_at;                 // point → the link it is interior to, or none
  std::vector<double> along;                        // point → free time from its link's `from` (s)
  std::vector<double> along_m;                      // point → metres from its link's `from`
  std::vector<char> joinable;                       // point → may a place join here
  std::size_t junctions() const { return point_of.size(); }
  std::size_t other(std::size_t link, std::size_t j) const { return links[link].from == j ? links[link].to : links[link].from; }
};

namespace detail {

inline std::uint64_t cell_key(double x, double y, double size) {
  const auto a = static_cast<std::int64_t>(std::floor(x / size)), b = static_cast<std::int64_t>(std::floor(y / size));
  return (static_cast<std::uint64_t>(a + (1LL << 30)) << 32) ^ static_cast<std::uint64_t>(b + (1LL << 30));
}

}  // namespace detail

// The contracted network of the roads.
inline Graph graph_of(const std::vector<Road>& roads, GraphOptions o = {}) {
  const char* who = "urban_risk_management::lifelines::graph_of";
  if (roads.empty()) throw std::invalid_argument(std::string(who) + ": at least one road");
  std::vector<ua::Way> ways;
  for (const Road& r : roads) {
    if (r.xy.size() % 2 || r.xy.size() < 4) throw std::invalid_argument(std::string(who) + ": a road has at least two points, in pairs");
    if (!(r.speed > 0 && r.width > 0)) throw std::invalid_argument(std::string(who) + ": a road's speed and width are > 0");
    ways.push_back({r.xy, r.speed});
  }
  ua::Network n = ua::network_of(ways, o.snap);
  if (o.stitch > 0) {
    ua::stitch_ends(n, o.stitch, o.join_speed);
    ua::stitch_to_ways(n, o.stitch, o.join_speed);
  }
  Graph g;
  g.x = n.x;
  g.y = n.y;
  const std::size_t N = n.x.size();
  g.joinable = ua::joinable(n, o.share);
  // Pieces: one per pair of neighbouring points, the least time where two ways share it.
  struct Piece { std::size_t u, v; double length, time, width; std::size_t road; };
  std::map<std::pair<std::size_t, std::size_t>, double> least;
  for (std::size_t u = 0; u < N; u++)
    for (const auto& a : n.arcs[u]) {
      if (a.to == u) continue;
      const auto key = std::minmax(u, a.to);
      auto it = least.find(key);
      if (it == least.end() || a.weight < it->second) least[key] = a.weight;
    }
  // Each road's segments in cells, to find the road a piece lies on.
  const double cell = 25;
  std::unordered_map<std::uint64_t, std::vector<std::pair<std::size_t, std::size_t>>> cells;  // (road, segment)
  for (std::size_t r = 0; r < roads.size(); r++)
    for (std::size_t i = 0; i + 3 < roads[r].xy.size(); i += 2) {
      const double x0 = roads[r].xy[i], y0 = roads[r].xy[i + 1], x1 = roads[r].xy[i + 2], y1 = roads[r].xy[i + 3];
      const int steps = std::max(1, static_cast<int>(std::ceil(std::hypot(x1 - x0, y1 - y0) / (0.5 * cell))));
      std::uint64_t last = 0;
      for (int k = 0; k <= steps; k++) {
        const double t = static_cast<double>(k) / steps;
        const auto c = detail::cell_key(x0 + t * (x1 - x0), y0 + t * (y1 - y0), cell);
        if (k == 0 || c != last) cells[c].push_back({r, i});
        last = c;
      }
    }
  const auto road_at = [&](double px, double py) {
    const auto it = cells.find(detail::cell_key(px, py, cell));
    if (it == cells.end()) return none;
    double best = 0.75;
    std::size_t at = none;
    for (const auto& [r, i] : it->second) {
      const double x0 = roads[r].xy[i], y0 = roads[r].xy[i + 1], dx = roads[r].xy[i + 2] - x0, dy = roads[r].xy[i + 3] - y0, L2 = dx * dx + dy * dy;
      const double t = L2 > 0 ? std::clamp(((px - x0) * dx + (py - y0) * dy) / L2, 0.0, 1.0) : 0.0;
      const double d = std::hypot(px - x0 - t * dx, py - y0 - t * dy);
      if (d < best) best = d, at = r;
    }
    return at;
  };
  std::vector<Piece> pieces;
  std::vector<std::vector<std::size_t>> at_point(N);
  for (const auto& [key, time] : least) {
    const auto [u, v] = key;
    const double length = std::hypot(g.x[v] - g.x[u], g.y[v] - g.y[u]);
    const std::size_t r = road_at((g.x[u] + g.x[v]) / 2, (g.y[u] + g.y[v]) / 2);
    pieces.push_back({u, v, length, time, r == none ? o.join_width : roads[r].width, r});
    at_point[u].push_back(pieces.size() - 1);
    at_point[v].push_back(pieces.size() - 1);
  }
  g.junction_at.assign(N, none);
  g.link_at.assign(N, none);
  g.along.assign(N, 0);
  g.along_m.assign(N, 0);
  const auto make_junction = [&](std::size_t p) {
    if (g.junction_at[p] == none) {
      g.junction_at[p] = g.point_of.size();
      g.point_of.push_back(p);
      g.incident.emplace_back();
    }
    return g.junction_at[p];
  };
  for (std::size_t p = 0; p < N; p++)
    if (!at_point[p].empty() && at_point[p].size() != 2) make_junction(p);
  std::vector<char> used(pieces.size(), 0);
  const auto walk = [&](std::size_t start, std::size_t first) {
    Link L;
    L.from = g.junction_at[start];
    L.points.push_back(start);
    std::map<std::size_t, double> by_road;
    double width = inf;
    std::size_t prev = start, piece = first;
    for (;;) {
      used[piece] = 1;
      const Piece& P = pieces[piece];
      const std::size_t next = P.u == prev ? P.v : P.u;
      L.length += P.length;
      L.time += P.time;
      width = std::min(width, P.width);
      by_road[P.road] += P.length;
      L.points.push_back(next);
      if (g.junction_at[next] != none) break;
      const std::size_t a = at_point[next][0], b = at_point[next][1];
      const std::size_t onward = a == piece ? b : a;
      if (used[onward]) {  // a loop back to itself through points of degree two: close it at this point
        make_junction(next);
        break;
      }
      prev = next;
      piece = onward;
    }
    L.to = g.junction_at[L.points.back()];
    L.width = width;
    double most = -1;
    for (const auto& [r, len] : by_road)
      if (len > most) most = len, L.road = r;
    const std::size_t id = g.links.size();
    double t = 0, m = 0;
    for (std::size_t k = 1; k + 1 < L.points.size(); k++) {
      // interior points: times along by the pieces walked
      const std::size_t p0 = L.points[k - 1], p1 = L.points[k];
      for (std::size_t pc : at_point[p1])
        if ((pieces[pc].u == p0 && pieces[pc].v == p1) || (pieces[pc].v == p0 && pieces[pc].u == p1)) {
          t += pieces[pc].time;
          m += pieces[pc].length;
          break;
        }
      g.link_at[p1] = id;
      g.along[p1] = t;
      g.along_m[p1] = m;
    }
    g.links.push_back(std::move(L));
    g.incident[g.links.back().from].push_back(id);
    if (g.links.back().to != g.links.back().from) g.incident[g.links.back().to].push_back(id);
  };
  for (std::size_t j = 0; j < g.point_of.size(); j++) {
    const std::size_t p = g.point_of[j];
    for (std::size_t pc : at_point[p])
      if (!used[pc]) walk(p, pc);
  }
  for (std::size_t pc = 0; pc < pieces.size(); pc++)  // loops of points of degree two alone
    if (!used[pc]) {
      make_junction(pieces[pc].u);
      walk(pieces[pc].u, pc);
    }
  return g;
}

// ---- places on the network -------------------------------------------------------------------------------------------------------

struct Attachment {
  std::size_t junction = none;  // a place at a junction
  std::size_t link = none;      // or on a link's interior
  double from = 0, to = 0;      // free times along the link to its ends (s)
  double gap = 0;               // the time across the gap to the network (s)
  double gap_m = 0;             // and its metres
};

// Finds the nearest joinable point quickly, by cells.
class Locator {
 public:
  explicit Locator(const Graph& g, double cell = 50) : g_(&g), cell_(cell) {
    for (std::size_t p = 0; p < g.x.size(); p++)
      if (g.joinable[p] && (g.junction_at[p] != none || g.link_at[p] != none)) cells_[detail::cell_key(g.x[p], g.y[p], cell)].push_back(p);
    if (cells_.empty()) throw std::invalid_argument("urban_risk_management::lifelines: the network joins nothing");
  }
  std::pair<std::size_t, double> nearest(double x, double y) const {
    std::size_t best = none;
    double bd = inf;
    for (int ring = 0; ring < 400; ring++) {
      for (int di = -ring; di <= ring; di++)
        for (int dj = -ring; dj <= ring; dj++) {
          if (std::max(std::abs(di), std::abs(dj)) != ring) continue;
          const auto it = cells_.find(detail::cell_key(x + di * cell_, y + dj * cell_, cell_));
          if (it == cells_.end()) continue;
          for (std::size_t p : it->second) {
            const double d = std::hypot(g_->x[p] - x, g_->y[p] - y);
            if (d < bd) bd = d, best = p;
          }
        }
      if (best != none && bd <= ring * cell_) break;
    }
    if (best == none) throw std::invalid_argument("urban_risk_management::lifelines: no point of the network within 20 km of a place");
    return {best, bd};
  }

 private:
  const Graph* g_;
  double cell_;
  std::unordered_map<std::uint64_t, std::vector<std::size_t>> cells_;
};

inline Attachment attach(const Graph& g, const Locator& locate, double x, double y, double gap_speed = 1.34) {
  const auto [p, d] = locate.nearest(x, y);
  Attachment a;
  a.gap_m = d;
  a.gap = d / gap_speed;
  if (g.junction_at[p] != none) a.junction = g.junction_at[p];
  else {
    a.link = g.link_at[p];
    a.from = g.along[p];
    a.to = g.links[a.link].time - g.along[p];
  }
  return a;
}

// ---- times under a state ---------------------------------------------------------------------------------------------------------

using State = std::vector<double>;  // per link, the share of its speed it keeps

struct Field {
  std::vector<double> d;           // per junction: time to the nearest source (s), +inf where none is reached
  std::vector<std::size_t> via;    // the link it is reached by (or the link of the source seeding it); none at a source's junction
  std::vector<std::size_t> parent; // the junction it is reached from; none where a source seeds it
};

namespace detail {

inline double link_time(const Graph& g, const State& phi, std::size_t l) { return phi[l] > 0 ? g.links[l].time / phi[l] : inf; }

// Seeds of the sources at the junctions, with the link each seed comes along (none from a source at the junction itself).
struct Seed { std::size_t junction; double offset; std::size_t link; };
inline std::vector<Seed> seeds(const Graph& g, const State& phi, const std::vector<Attachment>& sources, std::size_t without = none) {
  std::vector<Seed> out;
  for (const Attachment& s : sources) {
    if (s.junction != none) out.push_back({s.junction, s.gap, none});
    else if (s.link != without && phi[s.link] > 0) {
      out.push_back({g.links[s.link].from, s.gap + s.from / phi[s.link], s.link});
      out.push_back({g.links[s.link].to, s.gap + s.to / phi[s.link], s.link});
    }
  }
  return out;
}

}  // namespace detail

inline Field field(const Graph& g, const State& phi, const std::vector<Attachment>& sources, double limit = inf) {
  const std::size_t J = g.junctions();
  mc::WeightedAdjacency adj(J);
  for (std::size_t l = 0; l < g.links.size(); l++) {
    const double t = detail::link_time(g, phi, l);
    if (!std::isfinite(t) || g.links[l].from == g.links[l].to) continue;
    adj[g.links[l].from].push_back({g.links[l].to, t});
    adj[g.links[l].to].push_back({g.links[l].from, t});
  }
  const auto S = detail::seeds(g, phi, sources);
  std::vector<std::pair<std::size_t, double>> src;
  std::vector<double> seed_at(J, inf);
  std::vector<std::size_t> seed_link(J, none);  // the link of the least seed at each junction (none from a source at the junction)
  for (const auto& s : S) {
    src.push_back({s.junction, s.offset});
    if (s.offset < seed_at[s.junction]) seed_at[s.junction] = s.offset, seed_link[s.junction] = s.link;
  }
  Field F;
  F.via.assign(J, none);
  F.parent.assign(J, none);
  if (src.empty()) {
    F.d.assign(J, inf);
    return F;
  }
  const auto P = mc::dijkstra(adj, src, limit);
  F.d = P.distance;
  for (std::size_t v = 0; v < J; v++) {
    if (!std::isfinite(F.d[v])) continue;
    if (P.parent[v] != mc::ShortestPaths::none) {
      const std::size_t u = P.parent[v];
      F.parent[v] = u;
      double best = inf;
      for (std::size_t l : g.incident[v])
        if (g.other(l, v) == u && detail::link_time(g, phi, l) < best) best = detail::link_time(g, phi, l), F.via[v] = l;
    } else F.via[v] = seed_link[v];
  }
  return F;
}

// A place's time under a field: across its gap, then from its junction or along its link (to an end, or to a source on the link).
inline double time_of(const Graph& g, const State& phi, const Field& F, const Attachment& p, const std::vector<std::vector<std::size_t>>& sources_on,
                      const std::vector<Attachment>& sources, std::size_t without = none) {
  if (p.junction != none) return p.gap + F.d[p.junction];
  if (p.link == without || !(phi[p.link] > 0)) return inf;
  const Link& L = g.links[p.link];
  double t = std::min(F.d[L.from] + p.from / phi[p.link], F.d[L.to] + p.to / phi[p.link]);
  for (std::size_t k : sources_on[p.link]) t = std::min(t, sources[k].gap + std::fabs(p.from - sources[k].from) / phi[p.link]);
  return p.gap + t;
}

// ---- the system: people reaching a service ---------------------------------------------------------------------------------------

struct Measure {
  double threshold = 30 * 60;  // s: beyond it a person counts as beyond reach
  double cap = 120 * 60;       // s: the time counted for a person who reaches nothing, and the most any time counts
};

struct Place {
  double x = 0, y = 0;
  double weight = 0;  // people, or a service's capacity
};

struct Reading {
  std::vector<double> time;      // per place, s (+inf: reaches nothing)
  double loss = 0;               // person-minutes against the intact network
  double beyond = 0;             // people beyond the threshold
  double isolated = 0;           // people who reach nothing
  double mean_minutes = 0;       // the people's mean time, capped, in minutes
};

class Lifelines {
 public:
  Lifelines(Graph g, const std::vector<Place>& people, const std::vector<Place>& services, Measure m = {}, double gap_speed = 1.34)
      : g_(std::move(g)), m_(m) {
    if (people.empty() || services.empty()) throw std::invalid_argument("urban_risk_management::lifelines: people and at least one service");
    if (!(m.threshold > 0 && m.cap >= m.threshold)) throw std::invalid_argument("urban_risk_management::lifelines: a threshold > 0 and a cap at least as long");
    const Locator locate(g_);
    for (const Place& p : people) {
      if (!(p.weight >= 0 && std::isfinite(p.weight))) throw std::invalid_argument("urban_risk_management::lifelines: people are finite and ≥ 0");
      people_.push_back(attach(g_, locate, p.x, p.y, gap_speed));
      weight_.push_back(p.weight);
    }
    for (const Place& s : services) services_.push_back(attach(g_, locate, s.x, s.y, gap_speed));
    sources_on_.assign(g_.links.size(), {});
    for (std::size_t k = 0; k < services_.size(); k++)
      if (services_[k].link != none) sources_on_[services_[k].link].push_back(k);
    places_on_.assign(g_.links.size(), {});
    places_at_.assign(g_.junctions(), {});
    for (std::size_t i = 0; i < people_.size(); i++) {
      if (people_[i].link != none) places_on_[people_[i].link].push_back(i);
      else places_at_[people_[i].junction].push_back(i);
    }
    const State intact(g_.links.size(), 1.0);
    const Field F = field(g_, intact, services_);
    t0_.resize(people_.size());
    for (std::size_t i = 0; i < people_.size(); i++) t0_[i] = time_of(g_, intact, F, people_[i], sources_on_, services_);
  }

  const Graph& graph() const { return g_; }
  std::size_t links() const { return g_.links.size(); }
  const std::vector<Attachment>& people() const { return people_; }
  const std::vector<Attachment>& services() const { return services_; }
  const std::vector<double>& intact_times() const { return t0_; }
  const Measure& measure() const { return m_; }

  double contribution(std::size_t i, double t) const { return weight_[i] * (std::min(t, m_.cap) - std::min(t0_[i], m_.cap)) / 60; }

  Reading read(const State& phi) const {
    check(phi);
    const Field F = field(g_, phi, services_);
    Reading r;
    double people = 0, sum = 0;
    for (std::size_t i = 0; i < people_.size(); i++) {
      const double t = time_of(g_, phi, F, people_[i], sources_on_, services_);
      r.time.push_back(t);
      r.loss += contribution(i, t);
      if (t > m_.threshold) r.beyond += weight_[i];
      if (!std::isfinite(t)) r.isolated += weight_[i];
      people += weight_[i];
      sum += weight_[i] * std::min(t, m_.cap);
    }
    r.mean_minutes = people > 0 ? sum / people / 60 : 0;
    return r;
  }

  double loss(const State& phi) const { return read(phi).loss; }

  // The loss with each link in turn closed, at the state phi whose loss is `at`: only links that carry a shortest path, or on which
  // people or services sit, are asked, and for each only what lies beneath it is recomputed.
  std::vector<double> failed_all(const State& phi, double at) const {
    check(phi);
    const std::size_t J = g_.junctions(), K = g_.links.size();
    const Field F = field(g_, phi, services_);
    std::vector<double> time(people_.size());
    for (std::size_t i = 0; i < people_.size(); i++) time[i] = time_of(g_, phi, F, people_[i], sources_on_, services_);
    // The forest: children of each junction by the link that reaches them.
    std::vector<std::vector<std::size_t>> children(J);
    std::vector<std::vector<std::size_t>> rooted_on(K);  // junctions whose `via` is the link
    for (std::size_t v = 0; v < J; v++) {
      if (F.via[v] != none) rooted_on[F.via[v]].push_back(v);
      if (F.parent[v] != none) children[F.parent[v]].push_back(v);
    }
    std::vector<double> out(K, at);
    std::vector<char> in(J, 0);
    std::vector<std::size_t> index(J, none);
    for (std::size_t c = 0; c < K; c++) {
      if (!(phi[c] > 0)) continue;
      if (rooted_on[c].empty() && places_on_[c].empty() && sources_on_[c].empty()) continue;
      // The junctions beneath c.
      std::vector<std::size_t> sub;
      for (std::size_t r : rooted_on[c]) {
        std::vector<std::size_t> stack{r};
        while (!stack.empty()) {
          const std::size_t v = stack.back();
          stack.pop_back();
          if (in[v]) continue;
          in[v] = 1;
          sub.push_back(v);
          for (std::size_t w : children[v]) stack.push_back(w);
        }
      }
      std::vector<double> nd(sub.size(), inf);
      if (!sub.empty()) {
        for (std::size_t k = 0; k < sub.size(); k++) index[sub[k]] = k;
        // Seeds: from outside neighbours by open links other than c, and from the services that reach a junction beneath.
        std::vector<std::pair<std::size_t, double>> src;
        mc::WeightedAdjacency adj(sub.size());
        for (std::size_t k = 0; k < sub.size(); k++) {
          const std::size_t v = sub[k];
          for (std::size_t l : g_.incident[v]) {
            if (l == c) continue;
            const double t = detail::link_time(g_, phi, l);
            if (!std::isfinite(t)) continue;
            const std::size_t z = g_.other(l, v);
            if (z == v) continue;
            if (in[z]) adj[k].push_back({index[z], t});
            else if (std::isfinite(F.d[z])) src.push_back({k, F.d[z] + t});
          }
        }
        for (const auto& s : detail::seeds(g_, phi, services_, c))
          if (in[s.junction]) src.push_back({index[s.junction], s.offset});
        if (!src.empty()) nd = mc::dijkstra(adj, src).distance;
      }
      // The people whose times may change: at junctions beneath, on links touching them, and on c itself.
      Field G = F;
      for (std::size_t k = 0; k < sub.size(); k++) G.d[sub[k]] = nd[k];
      double delta = 0;
      const auto redo = [&](std::size_t i) {
        const double t = time_of(g_, phi, G, people_[i], sources_on_, services_, c);
        delta += contribution(i, t) - contribution(i, time[i]);
      };
      for (std::size_t v : sub) {
        for (std::size_t i : places_at_[v]) redo(i);
        for (std::size_t l : g_.incident[v])
          if (l != c && !mark(l)) for (std::size_t i : places_on_[l]) redo(i);
      }
      for (std::size_t i : places_on_[c]) redo(i);
      clear_marks();
      out[c] = at + delta;
      for (std::size_t v : sub) in[v] = 0, index[v] = none;
    }
    return out;
  }

  rc::Assessment assess(const std::vector<rc::Scenario>& scenarios) const {
    return rc::assess(links(), scenarios, [this](const State& phi) { return loss(phi); },
                      [this](const State& phi, double at) { return failed_all(phi, at); });
  }

 private:
  void check(const State& phi) const {
    if (phi.size() != g_.links.size()) throw std::invalid_argument("urban_risk_management::lifelines: a state gives a share for each link");
  }
  mutable std::vector<char> marks_;
  mutable std::vector<std::size_t> marked_;
  bool mark(std::size_t l) const {
    if (marks_.size() != g_.links.size()) marks_.assign(g_.links.size(), 0);
    if (marks_[l]) return true;
    marks_[l] = 1;
    marked_.push_back(l);
    return false;
  }
  void clear_marks() const {
    for (std::size_t l : marked_) marks_[l] = 0;
    marked_.clear();
  }

  Graph g_;
  Measure m_;
  std::vector<Attachment> people_, services_;
  std::vector<double> weight_, t0_;
  std::vector<std::vector<std::size_t>> sources_on_, places_on_, places_at_;
};

// ---- redundancy ------------------------------------------------------------------------------------------------------------------

struct Redundancy {
  double paths = 0;                 // link-disjoint paths from the area to the services (+inf if the area holds a service's junction)
  std::vector<std::size_t> cut;     // the links whose loss together cuts the area off: a least cut
  bool holds_service = false;
};

// Link-disjoint paths from the junctions of an area to the junctions at which services join (both ends of a service's link), through
// the links open at the state.
inline Redundancy redundancy(const Graph& g, const State& phi, const std::vector<std::size_t>& area, const std::vector<Attachment>& services) {
  if (phi.size() != g.links.size()) throw std::invalid_argument("urban_risk_management::lifelines::redundancy: a state gives a share for each link");
  const std::size_t J = g.junctions(), S = J, T = J + 1;
  std::vector<char> target(J, 0), source(J, 0);
  for (const Attachment& a : services) {
    if (a.junction != none) target[a.junction] = 1;
    else if (phi[a.link] > 0) target[g.links[a.link].from] = target[g.links[a.link].to] = 1;
  }
  Redundancy out;
  for (std::size_t j : area) {
    if (j >= J) throw std::invalid_argument("urban_risk_management::lifelines::redundancy: an area of junctions");
    source[j] = 1;
    if (target[j]) out.holds_service = true;
  }
  if (out.holds_service) {
    out.paths = inf;
    return out;
  }
  std::vector<mc::FlowArc> arcs;
  std::vector<std::size_t> link_of;
  for (std::size_t l = 0; l < g.links.size(); l++) {
    if (!(phi[l] > 0) || g.links[l].from == g.links[l].to) continue;
    arcs.push_back({g.links[l].from, g.links[l].to, 1});
    link_of.push_back(l);
    arcs.push_back({g.links[l].to, g.links[l].from, 1});
    link_of.push_back(l);
  }
  for (std::size_t j = 0; j < J; j++) {
    if (source[j]) arcs.push_back({S, j, inf}), link_of.push_back(none);
    if (target[j]) arcs.push_back({j, T, inf}), link_of.push_back(none);
  }
  const auto M = mc::max_flow(J + 2, arcs, S, T);
  out.paths = M.value;
  for (std::size_t a : M.cut)
    if (link_of[a] != none && std::find(out.cut.begin(), out.cut.end(), link_of[a]) == out.cut.end()) out.cut.push_back(link_of[a]);
  return out;
}

}  // namespace urban_risk_management::lifelines
