// Land use and transport, year by year, for a place (ADR 0094), as one composition any work may call: the Urban Science
// Workbench's plug-in "Land use and transport" answers its page with it, and a study may run it without the page. A place is its
// zones (cells of people and jobs) and its roads; nothing in it names a city. Like the toolbox (ADR 0080) and the designer
// (ADR 0089), it stands in the API so that no work depends on another.
//
//   run  {ways: [[speed m/s, capacity vehicles/h, x0, y0, x1, y1, …]], zones: [[x, y, households, jobs]], years,
//         scenario: {year, closed: [way], widened: [[way, factor]], built: [[zone, factor]], jobs: [[zone, added]]},
//         parameters: {beta, lambda, alpha, peak, walk, terminal, headroom, speed, depreciation, outside, gap}}
//       the zones' households, rents (an index, 100 in the base year), stock and accessibility, and the roads' loads in the
//       peak hour, year by year, as things stand and with the scenario
//
// The network. The ways, at their speeds, joined where they share a point and at loose ends a tile cut (urban::access), are
// contracted to chains between junctions and the zones' points: each chain two arcs of the Bureau of Public Roads, its time the
// sum of its pieces' and its capacity the least of theirs, in workers an hour (vehicles an hour over the peak hour's share of the
// workers on the roads). One-way streets are not distinguished. A zone joins the network at its nearest point in a part holding
// 1% of it, walking the gap; a journey adds that walk at each end and a terminal time.
//
// The year. urban::luti on that network, revealed from the zones' households as they stand (rents all at the rent at which
// building pays, so that nothing is built until something changes). Each zone's capacity is its stock times (1 + headroom),
// unless the scenario builds more there. M, the households who consider the place, is the households times (1 + outside).
// Evans's combined equilibrium is solved each year to the declared gap (10⁻³ by default) or to a number of iterations (1,500),
// whichever comes first, and each year says the gap it reached; a year that repeats the one before, with
// no event to come, is copied (a fixed point of the year's map), so that only the years a change moves are solved.
//
// Coordinates are metres of a local frame (east, north); times minutes.

#pragma once

#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>
#include <urban/luti/model.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <science/json.hpp>

namespace science::luti {

namespace ua = urban::access;
namespace ul = urban::luti;
namespace ng = nomos::games;
namespace ni = nomos::interaction;

struct Chain {
  std::size_t a = 0, b = 0;            // the kept nodes it joins (indices of the contracted network)
  double minutes = 0, capacity = 0;    // capacity in vehicles an hour
  std::vector<std::size_t> ways;       // the ways whose pieces it runs along
};

struct Place {
  ng::Network network;                 // the contracted network, two arcs a chain (2k: a → b, 2k + 1: b → a)
  std::vector<Chain> chains;
  std::vector<std::size_t> node;       // zone → its kept node
  std::vector<double> gap;             // zone → metres walked to the network
  std::size_t points = 0, junctions = 0;
};

// The ways' network contracted to chains between junctions and the zones' points.
inline Place place(const Json& ways, const std::vector<std::pair<double, double>>& zones, double peak) {
  std::vector<ua::Way> w;
  std::vector<double> capacity;
  for (const auto& row : ways.as_array()) {
    const auto& r = row.as_array();
    if (r.size() < 6) continue;
    ua::Way way;
    way.speed = r[0].as_number();
    capacity.push_back(r[1].as_number());
    for (std::size_t i = 2; i + 1 < r.size(); i += 2) way.xy.push_back(r[i].as_number()), way.xy.push_back(r[i + 1].as_number());
    w.push_back(std::move(way));
  }
  if (w.empty()) throw std::invalid_argument("luti: no roads");
  ua::Network n = ua::network_of(w, 1.0);
  // The pieces of the ways, undirected, each with its way.
  struct Piece { std::size_t u, v; double seconds, capacity; std::size_t way; };
  std::vector<Piece> pieces;
  std::set<std::pair<std::size_t, std::size_t>> made;
  for (std::size_t k = 0; k < w.size(); k++) {
    if (!(w[k].speed > 0)) continue;
    const auto& pts = n.points[k];
    for (std::size_t i = 1; i < pts.size(); i++) {
      const std::size_t u = pts[i - 1], v = pts[i];
      if (u == v) continue;
      pieces.push_back({u, v, std::hypot(n.x[v] - n.x[u], n.y[v] - n.y[u]) / w[k].speed, capacity[k], k});
      made.insert({std::min(u, v), std::max(u, v)});
    }
  }
  // Loose ends a tile cut, joined at a road's speed; the joins carry what the roads they join carry.
  ua::stitch_ends(n, 3.0, 8.0);
  for (std::size_t u = 0; u < n.x.size(); u++)
    for (const auto& a : n.arcs[u])
      if (u < a.to && !made.count({u, a.to})) pieces.push_back({u, a.to, a.weight, 1e9, static_cast<std::size_t>(-1)}), made.insert({u, a.to});
  Place out;
  out.points = n.x.size();
  // The zones joined.
  const auto ok = ua::joinable(n);
  std::vector<std::size_t> zone_point;
  for (const auto& [x, y] : zones) {
    const auto j = ua::join(n, x, y, ok);
    zone_point.push_back(j.point);
    out.gap.push_back(j.gap);
  }
  // Contraction: kept are the points of other than two neighbours and the zones' points.
  std::vector<std::vector<std::size_t>> touch(n.x.size());
  for (std::size_t k = 0; k < pieces.size(); k++) touch[pieces[k].u].push_back(k), touch[pieces[k].v].push_back(k);
  std::vector<char> kept(n.x.size(), 0);
  for (std::size_t p = 0; p < n.x.size(); p++) kept[p] = !touch[p].empty() && touch[p].size() != 2;
  for (const std::size_t p : zone_point) kept[p] = 1;
  std::vector<char> used(pieces.size(), 0);
  std::map<std::size_t, std::size_t> index;
  const auto id = [&](std::size_t p) {
    const auto it = index.find(p);
    if (it != index.end()) return it->second;
    const std::size_t k = index.size();
    index.emplace(p, k);
    return k;
  };
  const auto walk_chain = [&](std::size_t start, std::size_t first) {
    Chain c;
    c.capacity = std::numeric_limits<double>::infinity();
    std::size_t at = start, k = first;
    double seconds = 0;
    std::set<std::size_t> along;
    while (true) {
      used[k] = 1;
      seconds += pieces[k].seconds;
      c.capacity = std::min(c.capacity, pieces[k].capacity);
      if (pieces[k].way != static_cast<std::size_t>(-1)) along.insert(pieces[k].way);
      at = pieces[k].u == at ? pieces[k].v : pieces[k].u;
      if (kept[at]) break;
      std::size_t next = static_cast<std::size_t>(-1);
      for (const std::size_t q : touch[at])
        if (!used[q]) { next = q; break; }
      if (next == static_cast<std::size_t>(-1)) break;
      k = next;
    }
    if (!kept[at]) kept[at] = 1;
    c.a = id(start), c.b = id(at), c.minutes = seconds / 60;
    c.ways.assign(along.begin(), along.end());
    if (c.a != c.b) out.chains.push_back(std::move(c));
  };
  for (std::size_t p = 0; p < n.x.size(); p++)
    if (kept[p])
      for (const std::size_t k : touch[p])
        if (!used[k]) walk_chain(p, k);
  for (std::size_t k = 0; k < pieces.size(); k++)   // loops of two-neighbour points with nothing kept on them
    if (!used[k]) { kept[pieces[k].u] = 1; walk_chain(pieces[k].u, k); }
  out.junctions = index.size();
  out.network.nodes = index.size();
  for (const auto& c : out.chains) {
    const double cap = std::max(1.0, c.capacity) / peak;
    out.network.arcs.push_back(ng::bpr(c.a, c.b, std::max(c.minutes, 1e-3), cap));
    out.network.arcs.push_back(ng::bpr(c.b, c.a, std::max(c.minutes, 1e-3), cap));
  }
  for (const std::size_t p : zone_point) out.node.push_back(id(p));
  out.network.nodes = index.size();
  return out;
}

namespace detail {
// Six significant figures: enough for the differences a change makes, which may be a tenth of a per cent.
inline double q6(double x) {
  if (!std::isfinite(x) || x == 0) return std::isfinite(x) ? 0.0 : std::numeric_limits<double>::quiet_NaN();
  const double m = std::pow(10.0, std::floor(std::log10(std::fabs(x))) - 5);
  return std::round(x / m) * m;
}
// An arc's capacity from its time a + b x^p written as the Bureau of Public Roads' t₀ (1 + 0.15 (x/C)^p); +∞ when b = 0.
inline double capacity_of(const ng::Arc& a) { return a.b > 0 ? std::pow(a.a * 0.15 / a.b, 1 / a.power) : std::numeric_limits<double>::infinity(); }
inline double param(const Json& p, const char* key, double fallback) {
  const Json* v = p.find(key);
  return v && v->is_number() ? v->as_number() : fallback;
}
}  // namespace detail

inline Json run(const Json& r) {
  const auto started = std::chrono::steady_clock::now();
  const Json& P = r.has("parameters") ? r["parameters"] : Json::object();
  const double beta = detail::param(P, "beta", 0.07), lambda = detail::param(P, "lambda", 0.02), alpha = detail::param(P, "alpha", 1.0);
  const double peak = detail::param(P, "peak", 0.1), walk = detail::param(P, "walk", 1.34), terminal = detail::param(P, "terminal", 2.0);
  const double headroom = detail::param(P, "headroom", 0.5), speed = detail::param(P, "speed", 0.3), depreciation = detail::param(P, "depreciation", 0.0);
  const double outside = detail::param(P, "outside", 1.0), gap = detail::param(P, "gap", 1e-3);
  const int iterations = static_cast<int>(std::clamp(detail::param(P, "iterations", 1500), 10.0, 1e6));
  const int years = static_cast<int>(std::clamp(detail::param(r, "years", 10), 1.0, 30.0));
  std::vector<std::pair<double, double>> xy;
  std::vector<double> households, jobs;
  for (const auto& z : r["zones"].as_array()) {
    xy.push_back({z[0].as_number(), z[1].as_number()});
    households.push_back(std::max(0.0, z[2].as_number()));
    jobs.push_back(std::max(0.0, z[3].as_number()));
  }
  const std::size_t n = xy.size();
  if (n < 2) throw std::invalid_argument("luti: a place needs two zones or more");
  const Place pl = place(r["ways"], xy, peak);
  ul::Model m;
  m.market.classes = {{1.0, 1.0, {}}};
  m.market.alpha = alpha;
  m.market.mean_income = 1;
  m.market.income_power = 1;
  m.market.vacancy = 0.05;
  m.market.reference_rent = 1;
  double total = 0;
  for (const double h : households) total += h;
  if (!(total > 0)) throw std::invalid_argument("luti: no households");
  m.market.households = total * (1 + outside);
  m.transport.network = pl.network;
  m.transport.node = pl.node;
  m.transport.added = ni::Table(n, n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) m.transport.added(i, j) = (pl.gap[i] + pl.gap[j]) / walk / 60 + 2 * terminal;
  m.transport.gap = gap;
  m.transport.max_iterations = iterations;
  m.beta = beta;
  m.lambda = lambda;
  m.zones.resize(n);
  for (std::size_t i = 0; i < n; i++) m.zones[i].basic = jobs[i];
  double all_jobs = 0;
  for (const double e : jobs) all_jobs += e;
  if (!(all_jobs > 0)) for (std::size_t i = 0; i < n; i++) m.zones[i].basic = households[i];   // no workplaces: jobs where people are
  std::vector<double> h0 = households;
  for (double& h : h0) h = std::max(h, 1.0);
  const ul::Year base = ul::reveal(m, h0);
  for (std::size_t i = 0; i < n; i++) m.zones[i].capacity = m.zones[i].stock * (1 + headroom);
  m.development = {1.0, speed, depreciation};
  // The scenario's events.
  std::vector<ul::Event> events;
  const Json& S = r.has("scenario") ? r["scenario"] : Json::object();
  const int when = static_cast<int>(std::clamp(detail::param(S, "year", 1), 1.0, static_cast<double>(years)));
  std::set<std::size_t> closed_ways;
  std::map<std::size_t, double> widened;
  if (const Json* c = S.find("closed")) for (const auto& k : c->as_array()) closed_ways.insert(static_cast<std::size_t>(k.as_number()));
  if (const Json* c = S.find("widened")) for (const auto& k : c->as_array()) widened[static_cast<std::size_t>(k[0].as_number())] = k[1].as_number();
  for (std::size_t c = 0; c < pl.chains.size(); c++) {
    bool closed = false;
    double factor = 1;
    for (const std::size_t wk : pl.chains[c].ways) {
      if (closed_ways.count(wk)) closed = true;
      if (widened.count(wk)) factor = std::max(factor, widened[wk]);
    }
    if (!closed && factor == 1) continue;
    for (int dir = 0; dir < 2; dir++) {
      ul::Event e;
      e.kind = ul::Event::arc, e.year = when, e.index = 2 * c + dir;
      const auto& a = pl.network.arcs[2 * c + dir];
      const double cap = std::max(1.0, pl.chains[c].capacity) / peak * factor;
      e.road = closed ? ng::Arc{a.tail, a.head, 1e5, 0, 1} : ng::bpr(a.tail, a.head, std::max(pl.chains[c].minutes, 1e-3), cap);
      events.push_back(e);
    }
  }
  if (const Json* b = S.find("built"))
    for (const auto& k : b->as_array()) {
      const auto z = static_cast<std::size_t>(k[0].as_number());
      if (z >= n) continue;
      ul::Event e;
      e.kind = ul::Event::capacity, e.year = when, e.index = z, e.value = m.zones[z].stock * k[1].as_number();
      events.push_back(e);
    }
  if (const Json* jb = S.find("jobs"))
    for (const auto& k : jb->as_array()) {
      const auto z = static_cast<std::size_t>(k[0].as_number());
      if (z >= n) continue;
      ul::Event e;
      e.kind = ul::Event::basic, e.year = when, e.index = z, e.value = m.zones[z].basic + k[1].as_number();
      events.push_back(e);
    }
  // Both runs, from the revealed year.
  // Each year's arcs' capacities go with it, so that a road's load is over the capacity it had that year.
  struct Run { std::vector<ul::Year> years; std::vector<std::vector<double>> capacity; };
  const auto capacities = [](const ul::Model& mm) {
    std::vector<double> c;
    for (const auto& a : mm.transport.network.arcs) c.push_back(detail::capacity_of(a));
    return c;
  };
  // A year that repeats the one before it (to 10⁻⁹), with no event in the next, is a fixed point of the year's map: the next year
  // is copied instead of solved again. So the run as things stand is solved once, and the changed run from its change on.
  const auto same = [](const ul::Year& a, const ul::Year& b) {
    for (std::size_t i = 0; i < a.households.size(); i++) {
      const auto off = [](double u, double v) { return std::fabs(u - v) > 1e-9 * std::max(1.0, std::fabs(v)); };
      if (off(a.households[i], b.households[i]) || off(a.stock[i], b.stock[i]) || off(a.rents[i], b.rents[i]) ||
          (std::isfinite(a.access[i]) && off(a.access[i], b.access[i])))
        return false;
    }
    return true;
  };
  const auto go = [&](const std::vector<ul::Event>& ev) {
    ul::Model mm = m;
    Run out{{base}, {capacities(mm)}};
    for (int t = 1; t <= years; t++) {
      bool quiet = true;
      for (const auto& e : ev) quiet = quiet && e.year != t;
      ul::apply(mm, ev, t);
      const std::size_t k = out.years.size();
      if (quiet && k >= 2 && same(out.years[k - 1], out.years[k - 2])) {
        ul::Year y = out.years.back();
        y.year = t;
        out.years.push_back(std::move(y));
      } else {
        out.years.push_back(ul::step(mm, out.years.back()));
      }
      out.capacity.push_back(capacities(mm));
    }
    return out;
  };
  const auto plain = go({}), changed = go(events);
  const auto record = [&](const Run& run) {
    Json list = Json::array();
    for (std::size_t t = 0; t < run.years.size(); t++) {
      const auto& y = run.years[t];
      const auto& cap = run.capacity[t];
      Json z = Json::array();
      for (std::size_t i = 0; i < n; i++)
        z.push(Json::array({detail::q6(y.households[i]), detail::q6(base.rents[i] > 0 ? 100 * y.rents[i] / base.rents[i] : 0), detail::q6(y.stock[i]), detail::q6(y.access[i]), detail::q6(y.jobs[i])}));
      // Each way's load in the peak hour: its chains' greatest flow over capacity.
      std::map<std::size_t, double> load;
      if (!y.arc_flow.empty())
        for (std::size_t c = 0; c < pl.chains.size(); c++) {
          const double v = std::max(y.arc_flow[2 * c] / cap[2 * c], y.arc_flow[2 * c + 1] / cap[2 * c + 1]);
          for (const std::size_t wk : pl.chains[c].ways) load[wk] = std::max(load[wk], v);
        }
      Json loads = Json::array();
      for (const auto& [wk, v] : load)
        if (v > 0.05) loads.push(Json::array({static_cast<double>(wk), detail::q6(v)}));
      double tc = 0, tt = 0, located = 0;
      for (std::size_t k = 0; k < y.trips.v.size(); k++)
        if (y.trips.v[k] > 0) tc += y.trips.v[k] * y.costs.v[k], tt += y.trips.v[k];
      for (const double h : y.households) located += h;
      list.push(Json::object({{"year", y.year}, {"zones", z}, {"loads", loads}, {"located", detail::q6(located)}, {"outside", detail::q6(y.outside)},
                              {"commute", detail::q6(tt > 0 ? tc / tt : 0)}, {"gap", y.transport_gap}, {"iterations", y.transport_iterations}}));
    }
    return list;
  };
  const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
  return Json::object({{"plain", record(plain)}, {"changed", record(changed)}, {"events", static_cast<double>(events.size())},
                       {"network", Json::object({{"points", static_cast<double>(pl.points)}, {"nodes", static_cast<double>(pl.junctions)}, {"chains", static_cast<double>(pl.chains.size())}})},
                       {"households", total}, {"seconds", seconds}});
}

}  // namespace science::luti
