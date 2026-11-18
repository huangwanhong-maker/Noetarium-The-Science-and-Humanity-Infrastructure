// Checks of evacuation (evacuation.hpp).
//
// - A corridor of 400 m and 4 m wide from 2,000 people to one shelter, walking at 1 m/s with 1.3 people per metre a second:
//   * reach: everyone walks there freely in 400 s; within 300 s nobody, within 500 s everyone; a shelter of 1,500 houses 1,500;
//   * the bound: by step k at most (k − τ + 1) × 4 × 1.3 × step have arrived, and everyone by τ + ⌈2000 / (5.2 step)⌉ − 1 steps;
//   * the simulation discharges at the corridor's capacity: 99% are safe after about 420 s (the corridor, and an entry and an exit
//     of a cell each) + 0.99 × 2000 / 5.2 s, within 5%;
//     nobody is lost (the account closes to rounding), and it never runs ahead of the bound by more than 1% of the people.
// - Departures spread over ten minutes delay the bound's clearance and the simulation's past ten minutes.
// - On a lattice with three small shelters and safe ground: shelters admit no more than they hold, the routes change as they fill,
//   and the simulation stays within the bound (to 1% of the people) at every reading.
// - Short links are merged away; a closed link is left out.
// - Planning: the most housed within a time is submodular in the shelters open (every pair of nested sets and element on random
//   instances); the greedy choice of two reaches 1 − 1/e of the best pair, listed, and its bound lies above it.
// - Refusals: no sink, a departure curve that does not end at 1, a jam density too low for the capacity.

#include <disaster_risk/evacuation/evacuation.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace de = disaster_risk::evacuation;
namespace ll = urban_risk_management::lifelines;

namespace {
bool throws(auto&& f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
std::vector<ll::Road> lattice(int k, double step, double width) {
  std::vector<ll::Road> roads;
  for (int i = 0; i < k; i++) {
    ll::Road h{{}, 10, width, ""}, v{{}, 10, width, ""};
    for (int j = 0; j < k; j++) h.xy.push_back(j * step), h.xy.push_back(i * step), v.xy.push_back(i * step), v.xy.push_back(j * step);
    roads.push_back(h), roads.push_back(v);
  }
  return roads;
}
// The largest excess of the simulation over the bound at its readings.
double excess(const de::Simulation& S, const de::Bound& B) {
  double worst = 0;
  for (std::size_t k = 0; k < S.times.size(); k++) worst = std::max(worst, S.safe[k] - B.at(S.times[k]));
  return worst;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  ll::GraphOptions o;
  o.stitch = 0;
  de::Walking w;  // 1 m/s, 1.3 per metre a second, 5.4 per m²
  {
    ll::Road corridor{{}, 10, 4, "corridor"};
    for (int i = 0; i <= 8; i++) corridor.xy.push_back(50.0 * i), corridor.xy.push_back(0);
    const auto g = ll::graph_of({corridor}, o);
    const auto n = de::network(g, ll::State(g.links.size(), 1.0), w);
    const auto P = de::people_on(g, n, {{0, 0, 2000}}, w);
    const auto S = de::sinks_of(g, n, {{400, 0, 0}}, {}, w);
    const auto R = de::reach(n, P, S, w, 500);
    s.check("everyone walks to the shelter freely in 400 s", R.time[0] - 400, 1e-9);
    s.require("within 500 s everyone, within 300 s nobody", R.within == 2000 && de::reach(n, P, S, w, 300).within == 0);
    const auto S15 = de::sinks_of(g, n, {{400, 0, 1500}}, {}, w);
    s.check("a shelter of 1,500 houses 1,500", de::reach(n, P, S15, w, 500).housed - 1500, 1e-9);
    const double step = 10;
    const auto B = de::bound(n, P, S, w, de::Departure::at_once(), step, 1200);
    const double u = 4 * 1.3 * step;
    bool line = true;
    for (std::size_t k = 0; k < B.safe.size(); k++) line = line && std::fabs(B.safe[k] - std::min(2000.0, k >= 40 ? (k - 40 + 1) * u : 0.0)) < 1e-6;
    s.require("the bound: (k − τ + 1) × 4 × 1.3 × step by step k", line);
    s.check("everyone by τ + ⌈2000 / 52⌉ − 1 = 78 steps", B.clearance - 78 * step, 1e-9);
    const auto Sim = de::simulate(n, P, S, w, de::Departure::at_once(), 1500, 10, 60, 10);
    const double want = 400 + 2 * 10 + 0.99 * 2000 / 5.2;  // the entry and the exit are cells of 10 m each
    s.check("the simulation discharges at the corridor's capacity: 99% safe near 420 + 0.99 × 2000/5.2 s (relative)", Sim.clearance / want - 1, 0.05);
    s.require("and all but a millionth later, the scheme smearing the crowd's rear", Sim.all_safe >= Sim.clearance && std::isfinite(Sim.all_safe));
    s.check("nobody is lost: the account closes", Sim.residual, 1e-6);
    s.check("the simulation runs ahead of the bound by no more than 1% of the people", std::max(0.0, excess(Sim, B)) / 2000, 0.01);
    de::Departure spread{{0, 600}, {0, 1}, "spread over ten minutes"};
    const auto B2 = de::bound(n, P, S, w, spread, step, 2000);
    const auto Sim2 = de::simulate(n, P, S, w, spread, 2500, 10, 60, 10);
    s.require("departures over ten minutes delay the bound and the simulation past ten minutes",
              B2.clearance > 600 && Sim2.clearance > 600 && B2.clearance >= B.clearance && Sim2.clearance >= Sim.clearance - 1e-9);
  }
  {
    calx::random::Stream rng(37, 1, 2, 3);
    const auto g = ll::graph_of(lattice(6, 80, 5), o);
    const auto n = de::network(g, ll::State(g.links.size(), 1.0), w);
    std::vector<ll::Place> people;
    for (int i = 0; i < 40; i++) people.push_back({rng.uniform(0, 400), rng.uniform(0, 400), rng.uniform(20, 80)});
    const auto P = de::people_on(g, n, people, w);
    std::vector<char> safe(n.x.size(), 0);
    for (std::size_t v = 0; v < n.x.size(); v++) safe[v] = n.x[v] > 390 && n.y[v] > 390;
    const auto S = de::sinks_of(g, n, {{0, 0, 300}, {400, 0, 250}, {0, 400, 200}}, safe, w);
    const auto B = de::bound(n, P, S, w, de::Departure::at_once(), 10, 3000);
    const auto Sim = de::simulate(n, P, S, w, de::Departure::at_once(), 3000, 15, 120, 10);
    bool within = true;
    for (std::size_t k = 0; k < 3; k++) within = within && Sim.arrived[k] <= S.capacity[k] * (1 + 1e-6) + 1e-6;
    s.require("shelters admit no more than they hold, and the routes change as they fill", within && Sim.reroutes >= 1);
    s.check("the simulation stays within the bound at every reading (to 1% of the people)", std::max(0.0, excess(Sim, B)) / Sim.total, 0.01);
    s.check("and its account closes", Sim.residual / Sim.total, 1e-9);
  }
  {
    std::vector<ll::Road> roads = lattice(3, 100, 6);
    roads.push_back({{100, 100, 103, 100}, 10, 6, "stub"});
    roads.push_back({{103, 100, 150, 140}, 10, 6, "lane"});
    const auto g = ll::graph_of(roads, o);
    ll::State phi(g.links.size(), 1.0);
    const auto n = de::network(g, phi, w, 5);
    bool short_gone = true;
    for (const auto& e : n.edges) short_gone = short_gone && g.links[e.link].length >= 5;
    phi[0] = 0;
    const auto m = de::network(g, phi, w, 5);
    bool closed_gone = true;
    for (const auto& e : m.edges) closed_gone = closed_gone && e.link != 0;
    s.require("short links are merged away, a closed link is left out", short_gone && closed_gone && m.edges.size() + 1 <= n.edges.size());
  }
  {
    calx::random::Stream rng(41, 4, 5, 6);
    bool submodular = true;
    for (int trial = 0; trial < 6; trial++) {
      const auto g = ll::graph_of(lattice(5, 100, 5), o);
      const auto n = de::network(g, ll::State(g.links.size(), 1.0), w);
      std::vector<ll::Place> people, shelters;
      for (int i = 0; i < 25; i++) people.push_back({rng.uniform(0, 400), rng.uniform(0, 400), rng.uniform(10, 60)});
      for (int i = 0; i < 5; i++) shelters.push_back({rng.uniform(0, 400), rng.uniform(0, 400), rng.uniform(40, 200)});
      const auto P = de::people_on(g, n, people, w);
      const auto all = de::sinks_of(g, n, shelters, {}, w);
      const auto f = [&](unsigned mask) {
        de::Sinks open;
        for (std::size_t k = 0; k < 5; k++)
          if (mask >> k & 1u) open.node.push_back(all.node[k]), open.capacity.push_back(all.capacity[k]), open.name.push_back(all.name[k]);
        return open.node.empty() ? 0.0 : de::housed(n, P, open, w, 250);
      };
      for (unsigned A = 0; A < 32; A++)
        for (unsigned B = 0; B < 32; B++) {
          if ((A & B) != A) continue;
          for (unsigned e = 0; e < 5; e++) {
            if (B >> e & 1u) continue;
            const unsigned x = 1u << e;
            if (f(A | x) - f(A) < f(B | x) - f(B) - 1e-9) submodular = false;
          }
        }
      if (trial == 0) {
        const de::Sinks none_open;
        const auto Pl = de::plan(n, P, none_open, all, w, 250, 2);
        double best = 0;
        for (unsigned m = 0; m < 32; m++)
          if (__builtin_popcount(m) == 2) best = std::max(best, f(m));
        s.require("the greedy pair of shelters reaches 1 − 1/e of the best pair, listed, its bound above it",
                  Pl.total() >= Pl.guarantee * best - 1e-9 && Pl.bound >= best - 1e-9 && Pl.total() <= best + 1e-9);
      }
    }
    s.require("the most housed within a time is submodular in the shelters open, on 6 random instances", submodular);
  }
  {
    const auto g = ll::graph_of(lattice(3, 100, 5), o);
    const auto n = de::network(g, ll::State(g.links.size(), 1.0), w);
    const auto P = de::people_on(g, n, {{0, 0, 10}}, w);
    de::Walking bad;
    bad.jam = 1;
    s.require("refusals: no sink, a departure curve that does not end at 1, a jam density too low for the capacity",
              throws([&] { de::reach(n, P, de::Sinks{}, w, 100); }) && throws([] { de::Departure{{0, 1}, {0, 0.5}, ""}.check(); }) &&
                  throws([&] { bad.check(); }));
  }
  return s.done();
}
