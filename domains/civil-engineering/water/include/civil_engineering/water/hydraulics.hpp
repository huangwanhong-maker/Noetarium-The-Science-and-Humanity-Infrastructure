// The hydraulics of a water network: heads, pressures and flows at an instant, and their course over a period (theory note §3).
//
// Pipes lose head by friction and by fittings; pumps add it; junctions draw their demands; reservoirs and tanks hold their heads.
// The network is physis's network of conduits (decision 0029), solved by Todini and Pilati's global gradient algorithm, as EPANET
// solves it (Rossman et al. 2020, EPANET 2.2 Manual, §12.1–12.2). What names water is here:
//
//   Hazen–Williams   h = k C^{−1.852} d^{−4.871} L Q^{1.852}, with EPANET's coefficient k = 4.727 in ft and cfs, 10.659 in SI (the
//                    textbook SI value 10.67 differs by 0.1%) (Manual, Table 3.1)
//   Darcy–Weisbach   h = f(Re, ε/d) (L/d) v²/2g, f = 64/Re below Re = 2000, Swamee and Jain's explicit formula above 4000 (1976, J.
//                    Hydraul. Div. 102: 657–664), and between them a linear interpolation in Re (EPANET interpolates by Dunlop's
//                    cubic: the difference is confined to the transition)
//   minor losses     h = K v²/2g, g = 32.2 ft/s² as EPANET takes it
//   pumps            h = A − B Q^C through the curve's points: from one point (Q₁, H₁), EPANET's rule A = 1.33334 H₁ with no head
//                    at 2Q₁; from three points with the first at zero flow, the power curve through them; from more, the curve's
//                    segments; a pump does not run backwards (it closes), and a closed pump opens when the head against it falls
//                    below its shutoff head
//   check valves     a pipe that closes when its flow would reverse
//   demands          demand-driven (each junction draws its demand), or pressure-driven (Wagner, Shamir and Marks 1988, J. Water Resour.
//                    Plan. Manage. 114: 276–294): q = q_req √((p − p_min)/(p_req − p_min)) between p_min and p_req, none below and
//                    q_req above; written as an emitter to a fixed head z + p_min whose law is the inverse, steepened beyond its ends
//                    (slopes 10⁴ times the law's mean), so that delivery stays within 10⁻⁴ q_req of Wagner's law
//
// A part of the network with no path to a reservoir or tank through open links cannot be supplied: under demand-driven analysis its
// junctions are reported isolated, with no head; under pressure-driven analysis they deliver nothing.
//
// simulate() steps the network through the period at its hydraulic step: tanks' levels move by their net inflow, patterns change
// demands, and simple controls (a link opened or closed when a node falls below or rises above a value) are applied at each step.
// EPANET also shortens a step to the instant a tank fills or empties or a control's condition is met; this does not, and says so.

#pragma once

#include <civil_engineering/water/network.hpp>

#include <physis/discretisation/conduits.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace civil_engineering::water {

namespace pd = physis::discretisation;

// EPANET's constants, carried from ft and cfs into SI.
inline double epanet_hazen_williams() { return 4.727 * std::pow(0.3048, 4.871) * std::pow(0.028316846592, -1.852); }
inline constexpr double epanet_gravity = 32.2 * 0.3048;  // m/s²

struct Options {
  double time = 0;                     // s, for the patterns
  double gravity = epanet_gravity;
  double hazen_williams = -1;          // −1: EPANET's coefficient
  double viscosity = 1.1e-5 * 0.3048 * 0.3048;  // m²/s: EPANET's default 1.1×10⁻⁵ ft²/s
  bool pressure_driven = false;
  double minimum_pressure = 0, required_pressure = 20;  // m, for pressure-driven demands
  std::map<std::string, double> tank_levels;            // m, overriding the network's
  std::map<std::string, bool> open;                     // links' statuses, overriding the network's
  double tolerance = 1e-9;
};

struct Snapshot {
  std::vector<std::string> nodes, links;
  std::vector<double> head, pressure, demand, required;  // per node: m, m, m³/s delivered, m³/s asked
  std::vector<double> elevation;
  std::vector<double> flow, velocity, headloss;          // per link: m³/s, m/s, m (from − to)
  std::vector<bool> open;
  std::vector<std::string> isolated;
  int iterations = 0, status_rounds = 0;
  bool converged = false;
  double residual = 0, balance = 0;
  std::size_t junctions = 0;                             // the first nodes are the junctions
};

namespace detail {

inline double multiplier(const Network& net, const std::string& pattern, double t) {
  const std::string p = pattern.empty() ? (net.patterns.count(net.default_pattern) ? net.default_pattern : std::string()) : pattern;
  if (p.empty() || !net.patterns.count(p) || net.patterns.at(p).empty()) return 1.0;
  const auto& m = net.patterns.at(p);
  const auto k = static_cast<std::size_t>(std::floor(t / net.pattern_step + 1e-9)) % m.size();
  return m[k];
}

inline pd::LossLaw darcy_weisbach(double L, double d, double eps, double K, double nu, double g) {
  const double A = std::numbers::pi * d * d / 4;
  auto h = [=](double q) {
    const double v = std::fabs(q) / A, Re = v * d / nu;
    double f = 0;
    if (Re > 0) {
      auto sj = [&](double re) { const double t = std::log10(eps / (3.7 * d) + 5.74 / std::pow(re, 0.9)); return 0.25 / (t * t); };
      f = Re <= 2000 ? 64 / Re : Re >= 4000 ? sj(Re) : 0.032 + (sj(4000) - 0.032) * (Re - 2000) / 2000;
    }
    const double hv = q * std::fabs(q) / (2 * g * A * A);  // the velocity head, signed
    return (f * L / d + K) * hv;
  };
  return [h, A, L, d, nu, g](double q) {
    const double dq = 1e-7 * std::max(std::fabs(q), 1e-6);
    double slope = (h(q + dq) - h(q - dq)) / (2 * dq);
    if (std::fabs(q) < 1e-9) slope = std::max(slope, 32 * nu * L / (g * d * d * A));  // the laminar slope at rest
    return std::pair{h(q), slope};
  };
}

// A pump's curve as a law of loss (the negative of its gain), and its shutoff head.
inline std::pair<pd::LossLaw, double> pump_law(const Pump& p) {
  require(!p.curve.empty(), "the pump " + p.id + " has no curve (a pump of constant power is not read yet)");
  const auto& c = p.curve;
  if (c.size() == 1) {
    // EPANET: shutoff head 1.33334 H₁, no head at 2 Q₁, the power curve through the three points.
    const double q1 = c[0].first, h1 = c[0].second, h0 = 1.33334 * h1, q2 = 2 * q1;
    const double C = std::log((h0 - 0) / (h0 - h1)) / std::log(q2 / q1), B = (h0 - h1) / std::pow(q1, C);
    return {pd::gain_law(h0, B, C), h0};
  }
  if (c.size() == 3 && c[0].first == 0) {
    const double h0 = c[0].second, q1 = c[1].first, h1 = c[1].second, q2 = c[2].first, h2 = c[2].second;
    require(h0 > h1 && h1 > h2 && q2 > q1 && q1 > 0, "the pump " + p.id + "'s three points fall with flow");
    const double C = std::log((h0 - h2) / (h0 - h1)) / std::log(q2 / q1), B = (h0 - h1) / std::pow(q1, C);
    return {pd::gain_law(h0, B, C), h0};
  }
  // A curve of segments, extended linearly at its ends.
  auto pts = c;
  std::sort(pts.begin(), pts.end());
  const double shut = pts.front().first == 0 ? pts.front().second : pts[0].second + (pts[1].second - pts[0].second) / (pts[1].first - pts[0].first) * (0 - pts[0].first);
  return {[pts](double q) {
            std::size_t i = 0;
            while (i + 2 < pts.size() && q > pts[i + 1].first) i++;
            const double s = (pts[i + 1].second - pts[i].second) / (pts[i + 1].first - pts[i].first);
            const double gain = pts[i].second + s * (q - pts[i].first);
            return std::pair{-gain, std::max(-s, 1e-9)};
          },
          shut};
}

// Wagner's pressure-driven demand, inverted: the pressure above p_min that delivers q.
inline pd::LossLaw demand_law(double q_req, double span) {
  const double steep = 1e4 * span / q_req;
  return [q_req, span, steep](double q) {
    if (q < 0) return std::pair{steep * q, steep};
    if (q > q_req) return std::pair{span + steep * (q - q_req), steep};
    const double r = q / q_req;
    return std::pair{span * r * r, std::max(2 * span * r / q_req, span / q_req * 1e-6)};
  };
}

}  // namespace detail

inline Snapshot solve(const Network& net, const Options& o = {}) {
  const std::size_t J = net.junctions.size(), R = net.reservoirs.size(), T = net.tanks.size();
  const std::size_t N = J + R + T, P = net.pipes.size(), U = net.pumps.size();
  Snapshot s;
  s.junctions = J;
  std::map<std::string, std::size_t> node;
  for (std::size_t i = 0; i < J; i++) node[net.junctions[i].id] = i, s.nodes.push_back(net.junctions[i].id), s.elevation.push_back(net.junctions[i].elevation);
  for (std::size_t i = 0; i < R; i++) node[net.reservoirs[i].id] = J + i, s.nodes.push_back(net.reservoirs[i].id), s.elevation.push_back(net.reservoirs[i].head);
  for (std::size_t i = 0; i < T; i++) node[net.tanks[i].id] = J + R + i, s.nodes.push_back(net.tanks[i].id), s.elevation.push_back(net.tanks[i].elevation);
  auto at = [&](const std::string& id) {
    detail::require(node.count(id) > 0, "a link names a node that does not exist: " + id);
    return node.at(id);
  };
  const double k = o.hazen_williams > 0 ? o.hazen_williams : epanet_hazen_williams();
  // Fixed heads and demands at this instant.
  std::vector<std::pair<std::size_t, double>> fixed;
  for (std::size_t i = 0; i < R; i++) fixed.push_back({J + i, net.reservoirs[i].head * detail::multiplier(net, net.reservoirs[i].pattern.empty() ? "\x01" : net.reservoirs[i].pattern, o.time)});
  for (std::size_t i = 0; i < T; i++) {
    const auto& t = net.tanks[i];
    const double level = o.tank_levels.count(t.id) ? o.tank_levels.at(t.id) : t.level;
    fixed.push_back({J + R + i, t.elevation + level});
  }
  s.required.assign(N, 0.0);
  for (std::size_t i = 0; i < J; i++) s.required[i] = net.junctions[i].demand * net.demand_multiplier * detail::multiplier(net, net.junctions[i].pattern, o.time);
  // Links: their laws, and their statuses (pumps and check valves settle by rounds).
  std::vector<pd::LossLaw> law(P + U);
  std::vector<double> shutoff(P + U, 0.0);
  std::vector<std::size_t> from(P + U), to(P + U);
  std::vector<bool> open(P + U), checks(P + U, false);
  for (std::size_t i = 0; i < P; i++) {
    const auto& p = net.pipes[i];
    from[i] = at(p.from), to[i] = at(p.to);
    s.links.push_back(p.id);
    const double A = std::numbers::pi * p.diameter * p.diameter / 4, minor = p.minor_loss / (2 * o.gravity * A * A);
    if (net.headloss == "H-W") {
      auto friction = pd::power_law(k * std::pow(p.roughness, -1.852) * std::pow(p.diameter, -4.871) * p.length, 1.852);
      law[i] = minor > 0 ? pd::in_series({friction, pd::power_law(minor, 2)}) : friction;
    } else {
      law[i] = detail::darcy_weisbach(p.length, p.diameter, p.roughness, p.minor_loss, o.viscosity, o.gravity);
    }
    open[i] = o.open.count(p.id) ? o.open.at(p.id) : p.open;
    checks[i] = p.check_valve;
  }
  for (std::size_t i = 0; i < U; i++) {
    const auto& p = net.pumps[i];
    from[P + i] = at(p.from), to[P + i] = at(p.to);
    s.links.push_back(p.id);
    auto [l, h0] = detail::pump_law(p);
    law[P + i] = l, shutoff[P + i] = h0;
    open[P + i] = o.open.count(p.id) ? o.open.at(p.id) : p.open;
    checks[P + i] = true;
  }
  pd::ConduitSolution sol;
  for (s.status_rounds = 0; s.status_rounds < 12; s.status_rounds++) {
    // Which junctions reach a fixed head through open links.
    std::vector<std::vector<std::size_t>> adj(N);
    for (std::size_t e = 0; e < P + U; e++)
      if (open[e]) adj[from[e]].push_back(to[e]), adj[to[e]].push_back(from[e]);
    std::vector<char> reached(N, 0);
    std::vector<std::size_t> stack;
    for (const auto& [v, h] : fixed) reached[v] = 1, stack.push_back(v);
    while (!stack.empty()) {
      const std::size_t v = stack.back();
      stack.pop_back();
      for (std::size_t w : adj[v])
        if (!reached[w]) reached[w] = 1, stack.push_back(w);
    }
    pd::ConduitNetwork cn;
    cn.vertices = N;
    std::vector<std::size_t> edge_of(P + U, static_cast<std::size_t>(-1));
    for (std::size_t e = 0; e < P + U; e++) {
      if (!open[e]) continue;
      if (!o.pressure_driven && (!reached[from[e]] || !reached[to[e]])) continue;
      edge_of[e] = cn.add(from[e], to[e], law[e]);
    }
    std::vector<std::pair<std::size_t, double>> fx = fixed;
    std::vector<double> withdrawal(N, 0.0);
    std::vector<std::size_t> emitter(J, static_cast<std::size_t>(-1));
    if (o.pressure_driven) {
      const double span = o.required_pressure - o.minimum_pressure;
      detail::require(span > 0, "pressure-driven demand needs a required pressure above the minimum");
      for (std::size_t i = 0; i < J; i++) {
        if (!(s.required[i] > 0)) continue;
        const std::size_t w = cn.vertices++;
        fx.push_back({w, net.junctions[i].elevation + o.minimum_pressure});
        emitter[i] = cn.add(i, w, detail::demand_law(s.required[i], span));
      }
      withdrawal.assign(cn.vertices, 0.0);
      // A junction with no demand and no way to a fixed head is tied to its own elevation, so it is not refused. Nothing flows through
      // the tie, so its strength is immaterial; but it must not be so faint that, beside the pipes of the island (carrying nothing, their
      // slopes at the floor, their weights near 10⁹), the island's equations lose it to rounding: a conductance of one keeps them regular.
      for (std::size_t i = 0; i < J; i++)
        if (!reached[i] && emitter[i] == static_cast<std::size_t>(-1)) {
          const std::size_t w = cn.vertices++;
          withdrawal.push_back(0.0);
          fx.push_back({w, net.junctions[i].elevation});
          cn.add(i, w, pd::linear_law(1.0));
        }
    } else {
      for (std::size_t i = 0; i < J; i++)
        if (reached[i]) withdrawal[i] = s.required[i];
    }
    // Nodes no edge touches and no head fixes (isolated, demand-driven) are left out by fixing them at their elevation.
    std::vector<char> touched(cn.vertices, 0);
    for (const auto& e : cn.edges) touched[e.a] = touched[e.b] = 1;
    for (const auto& [v, h] : fx) touched[v] = 1;
    for (std::size_t v = 0; v < N; v++)
      if (!touched[v]) fx.push_back({v, s.elevation[v]});
    pd::ConduitOptions co;
    co.tolerance = o.tolerance;
    sol = pd::solve_conduits(cn, fx, withdrawal, co);
    s.iterations += sol.iterations;
    // Statuses: a pump or check valve with reversed flow closes; a closed one opens when it would carry flow forwards.
    bool changed = false;
    for (std::size_t e = 0; e < P + U; e++) {
      if (!checks[e]) continue;
      if (open[e] && edge_of[e] != static_cast<std::size_t>(-1) && sol.flow[edge_of[e]] < -1e-12) open[e] = false, changed = true;
      else if (!open[e] && (o.open.count(s.links[e]) == 0 || o.open.at(s.links[e]))) {
        const bool user_closed = e < P ? !net.pipes[e].open : !net.pumps[e - P].open;
        if (user_closed) continue;
        const double rise = sol.potential[to[e]] - sol.potential[from[e]];
        if ((e >= P && rise < shutoff[e]) || (e < P && rise < 0)) open[e] = true, changed = true;
      }
    }
    // Write the snapshot (also when unchanged, so the last round's answer stands).
    s.head.assign(N, NAN), s.pressure.assign(N, NAN), s.demand.assign(N, 0.0);
    s.isolated.clear();
    for (std::size_t v = 0; v < N; v++) {
      const bool supplied = o.pressure_driven || reached[v];
      if (!supplied) {
        if (v < J) s.isolated.push_back(s.nodes[v]);
        continue;
      }
      s.head[v] = sol.potential[v];
      s.pressure[v] = v < J ? sol.potential[v] - s.elevation[v] : v >= J + R ? sol.potential[v] - s.elevation[v] : 0.0;
    }
    for (std::size_t i = 0; i < J; i++) s.demand[i] = o.pressure_driven ? (emitter[i] != static_cast<std::size_t>(-1) ? std::clamp(sol.flow[emitter[i]], 0.0, s.required[i]) : 0.0)
                                                                         : (reached[i] ? s.required[i] : 0.0);
    s.flow.assign(P + U, 0.0), s.velocity.assign(P + U, 0.0), s.headloss.assign(P + U, 0.0), s.open = open;
    for (std::size_t e = 0; e < P + U; e++) {
      if (edge_of[e] == static_cast<std::size_t>(-1)) continue;
      s.flow[e] = sol.flow[edge_of[e]];
      s.headloss[e] = sol.potential[from[e]] - sol.potential[to[e]];
      if (e < P) s.velocity[e] = s.flow[e] / (std::numbers::pi * net.pipes[e].diameter * net.pipes[e].diameter / 4);
    }
    // Reservoirs' and tanks' net outflow as their (negative) demand, as EPANET reports it.
    for (std::size_t v = J; v < N; v++) {
      double inflow = 0;
      for (std::size_t e = 0; e < P + U; e++) inflow += (to[e] == v ? s.flow[e] : 0) - (from[e] == v ? s.flow[e] : 0);
      s.demand[v] = inflow;
    }
    s.converged = sol.converged, s.residual = sol.residual, s.balance = sol.balance;
    if (!changed) break;
  }
  return s;
}

// ---- over a period ----

struct Series {
  std::vector<double> times;            // s
  std::vector<Snapshot> snapshots;
  std::map<std::string, std::vector<double>> tank_level;  // m, at each time
  double tank_volume_error = 0;         // max |Σ tanks' volume change − Σ their net inflow × step| (rounding)
};

inline Series simulate(const Network& net, Options o = {}) {
  Series out;
  std::map<std::string, double> level;
  for (const auto& t : net.tanks) level[t.id] = o.tank_levels.count(t.id) ? o.tank_levels.at(t.id) : t.level;
  std::map<std::string, bool> status = o.open;
  const double step = net.hydraulic_step, end = net.duration;
  for (double t = 0; t <= end + 1e-9; t += step) {
    // Controls on the levels and pressures at the start of the step (pressures from the last snapshot).
    for (const auto& c : net.controls) {
      double value = NAN;
      if (level.count(c.node)) value = level.at(c.node);
      else if (!out.snapshots.empty()) {
        const auto& last = out.snapshots.back();
        for (std::size_t v = 0; v < last.nodes.size(); v++)
          if (last.nodes[v] == c.node) value = last.pressure[v];
      }
      if (std::isnan(value)) continue;
      if ((c.below && value < c.value) || (!c.below && value > c.value)) status[c.link] = c.open;
    }
    o.time = t, o.tank_levels = level, o.open = status;
    Snapshot s = solve(net, o);
    out.times.push_back(t);
    for (const auto& tk : net.tanks) out.tank_level[tk.id].push_back(level[tk.id]);
    // Tanks move by their net inflow over the step, held within their levels.
    for (std::size_t i = 0; i < net.tanks.size(); i++) {
      const auto& tk = net.tanks[i];
      const double inflow = s.demand[net.junctions.size() + net.reservoirs.size() + i];
      level[tk.id] = std::clamp(level[tk.id] + inflow * step / tk.area(), tk.min_level, tk.max_level);
    }
    out.snapshots.push_back(std::move(s));
  }
  return out;
}

}  // namespace civil_engineering::water
