// Land use and transport interacting, year by year: where households live and what they pay, the dwellings built, the jobs
// that serve them, the journeys to work and the congestion they make, each fed by the others with the lags they take
// (ADR 0094; the theory note, theory/land-use-and-transport.md).
//
// THE REGION. Zones i (any partition: cells, tracts, blocks), each with a stock of dwellings S_i, the most the rules allow
// K_i (from architecture's rules where lots are known; +∞ where unbounded), its own value δ⁰_i (what the zone offers that is
// not access: amenity, schools, prestige; revealed by inversion, `reveal`) and its basic employment E^B_i (what it sells
// beyond the region). Travel costs between zones come from a network whose arcs slow with their flow (nomos::games) or,
// without one, from a fixed table.
//
// A YEAR t, in the order of Wegener's speeds of urban change (Wegener 2004, "Overview of land-use transport models", in
// Hensher et al. (eds), Handbook of Transport Geography and Spatial Systems, Elsevier, 127–146: construction slow,
// location intermediate, travel fast):
//  1. Development. Each zone's stock moves toward its capacity at a rate that rises with the log of its last rent over the
//     rent at which building pays, S_i ← (1 − d) S_i + κ max(0, ln(r_i(t−1)/r_c)) (K_i − S_i), never beyond K_i (the
//     stock–flow model of DiPasquale and Wheaton 1996, Urban Economics and Real Estate Markets, Prentice Hall, ch. 10, in
//     its simplest form; where K is infinite, the room is the stock itself).
//  2. Location. Households choose among the zones with dwellings by urban::housing's market: segment i's own value is
//     δ⁰_i + λ A_i(t−1), λ the value of a unit of accessibility (in the units of the costs) and A last year's; the rents
//     clear the occupied stock (gross substitutes; unique). Households not located are the outside option's.
//  3. Jobs. Service jobs follow the households, b of them for each, spent at zone k with the production-constrained shares
//     toward the zones' basic jobs and households (Lowry 1964): E^S_k = b Σ_i H_i P^S_ik. E = E^B + E^S.
//  4. Travel. Workers O_i = w H_i go to the jobs, scaled to the workers' total (the rest of the jobs are taken by people
//     from beyond the region, not modelled), by the doubly constrained model at the network's congested costs: Evans's
//     combined equilibrium (nomos::interaction::combined), or the gravity model at the fixed costs.
//  5. Accessibility. A_i = (1/β) ln Σ_j E_j e^{−β c_ij}, the logsum of the jobs in units of cost: what step 2 of the next
//     year reads.
//
// The lags (location reads last year's access, development last year's rents) make the region a discrete dynamical system
// whose fixed points are stationary states, in which every equation holds with A(t) = A(t−1) and S(t) = S(t−1).
//
// Results that hold exactly (and are checked): households located plus the outside option's are M; trips are the workers;
// Σ E^S = b Σ H; with one class of household and the stock held, a change of last year's accessibility changes the log rent
// by exactly λ ΔA_i / α in every zone (capitalisation: the clearing rents are explicit for one class).
//
// Caveats. One type of dwelling per zone (the market's segments are zones); jobs are filled by the region's workers in
// proportion; one mode of travel, by the network or the table; households do not age, and M changes only by the scenario.

#pragma once

#include <urban/housing/market.hpp>

#include <nomos/games/congestion.hpp>
#include <nomos/interaction/combined.hpp>
#include <nomos/interaction/gravity.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace urban::luti {

namespace ni = nomos::interaction;
namespace ng = nomos::games;

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::luti: " + what);
}
}  // namespace detail

struct Zone {
  double stock = 0;                                          // dwellings
  double capacity = std::numeric_limits<double>::infinity();  // the most dwellings the rules allow
  double own = 0;                                            // δ⁰: the zone's own value, apart from access
  double basic = 0;                                          // basic employment
};

// The costs between zones: a network whose arcs slow with flow, each zone at a node (`node`), with `added` (terminal times,
// the walk to and from the network) added to every least time; or, when the network has no arcs, the fixed table `costs`.
struct Transport {
  ng::Network network;
  std::vector<std::size_t> node;
  ni::Table added;
  ni::Table costs;
  double gap = 1e-4;            // Evans's relative gap
  int max_iterations = 100000;
  bool congested() const { return !network.arcs.empty(); }
};

struct Development {
  double cost_rent = 1;      // r_c: the rent at which building pays, in the market's units
  double speed = 0;          // κ: the share of the room built a year per unit of ln(r/r_c); 0 holds the stock
  double depreciation = 0;   // d: the share of the stock lost a year
};

struct Model {
  std::vector<Zone> zones;
  housing::Market market;    // its classes, households M, α, π, vacancy and reference rent; its segments are the zones'
  Transport transport;
  Development development;
  double beta = 0.1;                 // the journey to work's rate per unit of cost
  double lambda = 0.1;               // the value of a unit of accessibility in the location utility
  double workers_per_household = 1;  // w
  double service_per_household = 0;  // b: service jobs per household; 0 makes every job basic
  double service_beta = 0.2;         // the service trips' rate per unit of cost
};

struct Year {
  int year = 0;
  std::vector<double> households, rents, stock, basic, service, jobs, access;   // by zone (rent 0 where no stock)
  ni::Table trips, costs;
  std::vector<double> arc_flow, arc_time;
  double outside = 0;               // households in the outside option
  double surplus = 0;               // households' expected greatest utility, in units of ln r (housing::surplus_log_rent)
  double clearing_residual = 0;
  double transport_gap = 0;          // Evans's relative gap, or Furness's residual without a network
  int transport_iterations = 0;
};

// What a scenario changes, from the start of a year: a zone's basic employment, capacity or own value; an arc (replaced,
// or appended when `index` is the number of arcs); the households M.
struct Event {
  enum Kind { basic, capacity, own, arc, households } kind = basic;
  int year = 1;
  std::size_t index = 0;
  double value = 0;
  ng::Arc road;
};

namespace detail {
inline ni::Table costs_of(const Model& m, const std::vector<double>& arc_time) {
  const auto& T = m.transport;
  if (!T.congested()) return T.costs;
  const auto s = ng::skims(T.network, arc_time, T.node, T.node);
  ni::Table c(m.zones.size(), m.zones.size());
  for (std::size_t i = 0; i < c.rows; i++)
    for (std::size_t j = 0; j < c.cols; j++) c(i, j) = s[i][j] + (T.added.v.empty() ? 0.0 : T.added(i, j));
  return c;
}

inline void check(const Model& m) {
  const std::size_t n = m.zones.size();
  require(n > 0, "a region has zones");
  require(m.beta > 0 && m.service_beta > 0, "the rates of the journeys are positive");
  require(m.workers_per_household > 0, "households have workers");
  for (const auto& z : m.zones) require(z.stock >= 0 && z.capacity >= 0 && z.basic >= 0, "a zone's stock, capacity and jobs are not negative");
  if (m.transport.congested()) {
    require(m.transport.node.size() == n, "a node for every zone");
    require(m.transport.added.v.empty() || (m.transport.added.rows == n && m.transport.added.cols == n), "the added costs are zones by zones");
  } else {
    require(m.transport.costs.rows == n && m.transport.costs.cols == n, "a cost table of zones by zones, or a network");
  }
}
}  // namespace detail

// The households, rents and surplus of the market whose segments are the zones with stock, at the given accessibility.
struct Located {
  std::vector<double> households, rents;
  double outside = 0, surplus = 0, residual = 0;
};

inline Located locate(const Model& m, const std::vector<double>& stock, const std::vector<double>& access, const std::vector<double>& last_rents) {
  const std::size_t n = m.zones.size();
  housing::Market market = m.market;
  market.segments.clear();
  market.distance_cost = 0;
  std::vector<std::size_t> zone_of;
  std::vector<double> start;
  for (std::size_t i = 0; i < n; i++) {
    if (!(stock[i] > 0)) continue;
    housing::Segment s;
    s.type = housing::apartment;
    s.area = i;
    s.supply = stock[i];
    s.base = m.zones[i].own + m.lambda * (std::isfinite(access[i]) ? access[i] : 0.0);
    market.segments.push_back(s);
    zone_of.push_back(i);
    start.push_back(last_rents.size() == n && last_rents[i] > 0 ? last_rents[i] : market.reference_rent);
  }
  detail::require(!market.segments.empty(), "some zone has dwellings");
  const auto cl = housing::clear(market, start);
  detail::require(cl.converged, "the rents do not clear the market");
  Located out;
  out.households.assign(n, 0.0);
  out.rents.assign(n, 0.0);
  const auto d = housing::demand(market, cl.rents);
  double located = 0;
  for (std::size_t k = 0; k < zone_of.size(); k++) out.households[zone_of[k]] = d[k], out.rents[zone_of[k]] = cl.rents[k], located += d[k];
  out.outside = market.households - located;
  out.surplus = housing::surplus_log_rent(market, cl.rents);
  out.residual = cl.residual;
  return out;
}

// Steps 3 to 5 of a year: the jobs, the journeys and the accessibility they give, from the households.
inline void travel(const Model& m, Year& y) {
  const std::size_t n = m.zones.size();
  y.service.assign(n, 0.0);
  if (m.service_per_household > 0) {
    std::vector<double> attraction(n);
    for (std::size_t k = 0; k < n; k++) attraction[k] = y.basic[k] + y.households[k];
    ni::Table c = m.transport.congested() ? detail::costs_of(m, y.arc_time.empty() ? ng::times(m.transport.network, std::vector<double>(m.transport.network.arcs.size(), 0.0)) : y.arc_time) : m.transport.costs;
    const auto P = ni::production_constrained(y.households, attraction, c, m.service_beta);
    const auto spent = P.col_sums();
    for (std::size_t k = 0; k < n; k++) y.service[k] = m.service_per_household * spent[k];
  }
  y.jobs.resize(n);
  double workers = 0, jobs = 0;
  std::vector<double> O(n), D(n);
  for (std::size_t i = 0; i < n; i++) y.jobs[i] = y.basic[i] + y.service[i], O[i] = m.workers_per_household * y.households[i], workers += O[i], jobs += y.jobs[i];
  detail::require(jobs > 0 && workers > 0, "a region with jobs and workers");
  for (std::size_t j = 0; j < n; j++) D[j] = y.jobs[j] * workers / jobs;
  if (m.transport.congested()) {
    ni::CombinedOptions o;
    o.gap = m.transport.gap;
    o.max_iterations = m.transport.max_iterations;
    if (!m.transport.added.v.empty()) o.added = &m.transport.added;
    const auto eq = ni::combined(m.transport.network, m.transport.node, m.transport.node, O, D, m.beta, o);
    y.trips = eq.flows;
    y.costs = eq.costs;
    y.arc_flow = eq.arc_flow;
    y.arc_time = eq.arc_time;
    y.transport_gap = eq.gap;
    y.transport_iterations = eq.iterations;
  } else {
    y.costs = m.transport.costs;
    const auto b = ni::doubly_constrained(O, D, y.costs, m.beta);
    y.trips = b.flows;
    y.transport_gap = b.residual;
    y.transport_iterations = b.iterations;
  }
  y.access = ni::logsum(y.jobs, y.costs, m.beta);
}

// The first year, from the zones' stock: the access of the basic jobs at free flow (or the table), the households located
// by it, then the jobs, journeys and access they make.
inline Year start(const Model& m, int year = 0) {
  detail::check(m);
  const std::size_t n = m.zones.size();
  Year y;
  y.year = year;
  y.stock.resize(n);
  y.basic.resize(n);
  for (std::size_t i = 0; i < n; i++) y.stock[i] = m.zones[i].stock, y.basic[i] = m.zones[i].basic;
  const auto free = m.transport.congested() ? detail::costs_of(m, ng::times(m.transport.network, std::vector<double>(m.transport.network.arcs.size(), 0.0))) : m.transport.costs;
  const auto a0 = ni::logsum(y.basic, free, m.beta);
  const auto loc = locate(m, y.stock, a0, {});
  y.households = loc.households, y.rents = loc.rents, y.outside = loc.outside, y.surplus = loc.surplus, y.clearing_residual = loc.residual;
  travel(m, y);
  return y;
}

// The base year of observed households and rents (by zone; rents all the reference rent where none are observed): each
// zone's stock is set so that its households occupy the natural share of it, H_i / (1 − v); the jobs, journeys and
// accessibility follow from the households; and each zone's own value δ⁰_i is what Berry's inversion of the market at those
// rents and households leaves after λ A_i. The year returned is then a fixed point of `step` while nothing changes and no
// stock is built: the market's rents are unique, and the observed ones clear it.
inline Year reveal(Model& m, const std::vector<double>& households, std::vector<double> rents = {}) {
  const std::size_t n = m.zones.size();
  detail::require(households.size() == n, "observed households for every zone");
  if (rents.empty()) rents.assign(n, m.market.reference_rent);
  detail::require(rents.size() == n, "an observed rent for every zone, or none");
  for (std::size_t i = 0; i < n; i++) m.zones[i].stock = households[i] / (1 - m.market.vacancy);
  detail::check(m);
  Year y;
  y.stock.resize(n);
  y.basic.resize(n);
  for (std::size_t i = 0; i < n; i++) y.stock[i] = m.zones[i].stock, y.basic[i] = m.zones[i].basic;
  y.households = households;
  travel(m, y);
  housing::Market market = m.market;
  market.segments.clear();
  market.distance_cost = 0;
  std::vector<std::size_t> zone_of;
  std::vector<double> r, occupied;
  for (std::size_t i = 0; i < n; i++) {
    if (!(households[i] > 0)) continue;
    housing::Segment seg;
    seg.type = housing::apartment;
    seg.area = i;
    seg.supply = y.stock[i];
    market.segments.push_back(seg);
    zone_of.push_back(i);
    r.push_back(rents[i]);
    occupied.push_back(households[i]);
  }
  const auto inv = housing::invert(market, r, occupied);
  detail::require(inv.converged, "the observed households and rents reveal no own values");
  for (std::size_t k = 0; k < zone_of.size(); k++) {
    const std::size_t i = zone_of[k];
    m.zones[i].own = inv.base[k] - m.lambda * (std::isfinite(y.access[i]) ? y.access[i] : 0.0);
  }
  y.rents.assign(n, 0.0);
  double located = 0;
  for (std::size_t k = 0; k < zone_of.size(); k++) y.rents[zone_of[k]] = r[k], located += occupied[k];
  y.outside = m.market.households - located;
  for (std::size_t k = 0; k < zone_of.size(); k++) market.segments[k].base = inv.base[k];
  y.surplus = housing::surplus_log_rent(market, r);
  y.clearing_residual = inv.residual;
  return y;
}

// Applies the events of a year to the model.
inline void apply(Model& m, const std::vector<Event>& events, int year) {
  for (const auto& e : events) {
    if (e.year != year) continue;
    switch (e.kind) {
      case Event::basic: detail::require(e.index < m.zones.size(), "an event's zone"); m.zones[e.index].basic = e.value; break;
      case Event::capacity: detail::require(e.index < m.zones.size(), "an event's zone"); m.zones[e.index].capacity = e.value; break;
      case Event::own: detail::require(e.index < m.zones.size(), "an event's zone"); m.zones[e.index].own = e.value; break;
      case Event::households: m.market.households = e.value; break;
      case Event::arc:
        detail::require(e.index <= m.transport.network.arcs.size(), "an event's arc");
        if (e.index == m.transport.network.arcs.size()) m.transport.network.arcs.push_back(e.road);
        else m.transport.network.arcs[e.index] = e.road;
        break;
    }
  }
}

// One year after `last`, the model's zones holding this year's basic jobs, capacities and own values.
inline Year step(Model& m, const Year& last) {
  detail::check(m);
  const std::size_t n = m.zones.size();
  Year y;
  y.year = last.year + 1;
  y.stock.resize(n);
  y.basic.resize(n);
  const auto& dv = m.development;
  for (std::size_t i = 0; i < n; i++) {
    const auto& z = m.zones[i];
    // Built only into the room the rules leave: a capacity lowered below the stock builds nothing and demolishes nothing.
    double s = last.stock[i] * (1 - dv.depreciation);
    if (dv.speed > 0 && last.rents[i] > dv.cost_rent) {
      const double room = std::isfinite(z.capacity) ? std::max(0.0, z.capacity - s) : s;
      s += std::min(1.0, dv.speed * std::log(last.rents[i] / dv.cost_rent)) * room;
    }
    y.stock[i] = s;
    m.zones[i].stock = y.stock[i];
    y.basic[i] = z.basic;
  }
  const auto loc = locate(m, y.stock, last.access, last.rents);
  y.households = loc.households, y.rents = loc.rents, y.outside = loc.outside, y.surplus = loc.surplus, y.clearing_residual = loc.residual;
  y.arc_time = last.arc_time;
  travel(m, y);
  return y;
}

// The years from the start through `years`, the events applied at the start of each.
inline std::vector<Year> run(Model m, const std::vector<Event>& events, int years) {
  apply(m, events, 0);
  std::vector<Year> out{start(m)};
  for (int t = 1; t <= years; t++) {
    apply(m, events, t);
    out.push_back(step(m, out.back()));
  }
  return out;
}

// The years until the region is stationary (the largest change of a zone's households and accessibility below `tolerance`,
// relative), or `max_years`; the last year returned.
inline Year stationary(Model m, double tolerance = 1e-10, int max_years = 10000) {
  Year y = start(m);
  for (int t = 1; t <= max_years; t++) {
    Year next = step(m, y);
    double change = 0;
    for (std::size_t i = 0; i < next.households.size(); i++) {
      change = std::max(change, std::fabs(next.households[i] - y.households[i]) / std::max(1.0, y.households[i]));
      if (std::isfinite(next.access[i])) change = std::max(change, std::fabs(next.access[i] - y.access[i]) / std::max(1.0, std::fabs(y.access[i])));
    }
    y = std::move(next);
    if (change < tolerance) break;
  }
  return y;
}

}  // namespace urban::luti
