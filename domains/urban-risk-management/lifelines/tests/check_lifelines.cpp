// Checks of lifelines (lifelines.hpp).
//
// - Two crossing roads with points every 50 m contract to five junctions and four links; a place at one end reaches a service at
//   the other in 200 m over the speed.
// - On random lattices of roads with random people, services and states (links slowed or closed), the loss with each link closed,
//   answered at once by pruning to the forest of shortest paths and recomputing what lies beneath each link, equals the loss
//   computed afresh with that link closed, for every link.
// - A bridge between two districts: closing it leaves the far side's people at the cap; its criticality is theirs; redundancy across
//   it is 1, the bridge its cut; on a lattice two corners are joined by 2 link-disjoint paths.
// - The intact network loses nothing; a closed link isolates the people on it.
// - The criticality over a hazard's scenarios through risk management agrees with asking each link afresh.
// - Refusals: a road of one point, a state of the wrong size.

#include <urban_risk_management/lifelines/lifelines.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace ll = urban_risk_management::lifelines;
namespace rc = risk_management::criticality;

namespace {

bool throws(auto&& f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

// A lattice of k × k streets, `step` metres apart, each a polyline through every crossing.
std::vector<ll::Road> lattice(int k, double step, double speed) {
  std::vector<ll::Road> roads;
  for (int i = 0; i < k; i++) {
    ll::Road h{{}, speed, 8, "street"}, v{{}, speed, 8, "street"};
    for (int j = 0; j < k; j++) {
      h.xy.push_back(j * step), h.xy.push_back(i * step);
      v.xy.push_back(i * step), v.xy.push_back(j * step);
    }
    roads.push_back(h);
    roads.push_back(v);
  }
  return roads;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  ll::GraphOptions o;
  o.stitch = 0;
  {
    ll::Road a{{0, 0, 50, 0, 100, 0, 150, 0, 200, 0}, 10, 6, "a"}, b{{100, -100, 100, -50, 100, 0, 100, 50, 100, 100}, 10, 6, "b"};
    const auto g = ll::graph_of({a, b}, o);
    s.require("two crossing roads contract to five junctions and four links", g.junctions() == 5 && g.links.size() == 4);
    const ll::Lifelines L(g, {{0, 0, 1}}, {{200, 0, 1}});
    s.check("a place at one end reaches a service at the other in 200 m over the speed", L.intact_times()[0] - 20, 1e-12);
  }
  {
    calx::random::Stream rng(17, 1, 2, 3);
    bool equal = true;
    double worst = 0;
    for (int trial = 0; trial < 12; trial++) {
      const auto g = ll::graph_of(lattice(7, 100, 10), o);
      std::vector<ll::Place> people, services;
      for (int i = 0; i < 60; i++) people.push_back({rng.uniform(-20, 620), rng.uniform(-20, 620), rng.uniform(1, 50)});
      for (int i = 0; i < 2 + trial % 3; i++) services.push_back({rng.uniform(0, 600), rng.uniform(0, 600), 1});
      ll::Measure m;
      m.threshold = 60, m.cap = 200;
      const ll::Lifelines L(g, people, services, m);
      for (int rep = 0; rep < 3; rep++) {
        ll::State phi(g.links.size(), 1.0);
        for (double& x : phi) {
          const double u = rng.uniform();
          x = u < 0.15 ? 0.0 : u < 0.35 ? rng.uniform(0.2, 0.9) : 1.0;
        }
        const double at = L.loss(phi);
        const auto F = L.failed_all(phi, at);
        for (std::size_t c = 0; c < g.links.size(); c++) {
          ll::State x = phi;
          x[c] = 0;
          const double fresh = L.loss(x);
          const double d = std::fabs(F[c] - fresh) / std::max(1.0, std::fabs(fresh));
          worst = std::max(worst, d);
          if (d > 1e-9) equal = false;
        }
      }
    }
    s.require("on random lattices and states, every link's closure answered at once equals the loss computed afresh", equal);
    s.check("the largest relative difference", worst, 1e-9);
  }
  {
    // Two districts of streets joined by one bridge from (300, 0) to (500, 0).
    std::vector<ll::Road> roads = lattice(4, 100, 10);
    for (auto r : lattice(4, 100, 10)) {
      for (std::size_t i = 0; i < r.xy.size(); i += 2) r.xy[i] += 500;
      roads.push_back(r);
    }
    roads.push_back({{300, 0, 400, 0, 500, 0}, 10, 8, "bridge"});
    const auto g = ll::graph_of(roads, o);
    std::vector<ll::Place> people{{50, 150, 10}, {650, 150, 7}, {720, 280, 5}};
    ll::Measure m;
    m.threshold = 120, m.cap = 600;
    const ll::Lifelines L(g, people, {{0, 0, 1}}, m);
    std::size_t bridge = ll::none;
    for (std::size_t l = 0; l < g.links.size(); l++)
      if (g.links[l].road == roads.size() - 1) bridge = l;
    ll::State phi(g.links.size(), 1.0);
    phi[bridge] = 0;
    const auto R = L.read(phi);
    s.require("closing the bridge isolates the far side's 12 people", R.isolated == 12 && std::isinf(R.time[1]) && std::isfinite(R.time[0]));
    double want = 0;
    for (std::size_t i = 1; i < 3; i++) want += people[i].weight * (600 - L.intact_times()[i]) / 60;
    s.check("its loss is theirs at the cap", R.loss - want, 1e-9);
    std::vector<std::size_t> far;
    for (std::size_t j = 0; j < g.junctions(); j++)
      if (g.x[g.point_of[j]] > 520) far.push_back(j);
    const auto Rd = ll::redundancy(g, ll::State(g.links.size(), 1.0), far, L.services());
    s.require("redundancy across the bridge is 1, the bridge its cut", Rd.paths == 1 && Rd.cut.size() == 1 && Rd.cut[0] == bridge);
    const auto g2 = ll::graph_of(lattice(5, 100, 10), o);
    std::vector<std::size_t> corner;
    for (std::size_t j = 0; j < g2.junctions(); j++)
      if (std::hypot(g2.x[g2.point_of[j]] - 100, g2.y[g2.point_of[j]] - 0) < 1) corner.push_back(j);
    const ll::Lifelines L2(g2, {{0, 0, 1}}, {{400, 400, 1}});
    const auto R2 = ll::redundancy(g2, ll::State(g2.links.size(), 1.0), corner, L2.services());
    s.require("on a lattice a junction of its edge is joined to the far corner by 3 link-disjoint paths", R2.paths == 3);
    s.check("the intact network loses nothing", L.loss(ll::State(g.links.size(), 1.0)), 1e-12);
  }
  {
    calx::random::Stream rng(23, 4, 5, 6);
    const auto g = ll::graph_of(lattice(6, 120, 9), o);
    std::vector<ll::Place> people;
    for (int i = 0; i < 40; i++) people.push_back({rng.uniform(0, 600), rng.uniform(0, 600), rng.uniform(1, 20)});
    ll::Measure m;
    m.threshold = 50, m.cap = 300;
    const ll::Lifelines L(g, people, {{0, 0, 1}, {600, 600, 1}}, m);
    std::vector<rc::Scenario> sc{{"dry", 0.6, {}}};
    for (int k = 0; k < 3; k++) {
      ll::State phi(g.links.size(), 1.0);
      for (double& x : phi) x = rng.uniform() < 0.2 ? 0.0 : 1.0;
      sc.push_back({"flood", 0.1 + 0.05 * k, phi});
    }
    const auto A = L.assess(sc);
    const auto B = rc::assess(L.links(), sc, [&](const ll::State& x) { return L.loss(x); });
    double diff = 0;
    for (std::size_t c = 0; c < L.links(); c++) diff = std::max(diff, std::fabs(A.components[c].criticality - B.components[c].criticality) + std::fabs(A.components[c].risk - B.components[c].risk));
    s.check("criticality and risk over a hazard's scenarios agree with asking each link afresh", diff, 1e-9);
    s.require("and the pruned assessment asks the loss far less often", A.evaluations < B.evaluations / 4);
  }
  {
    s.require("refusals: a road of one point, a state of the wrong size",
              throws([&] { ll::graph_of({{{0, 0}, 10, 6, ""}}, o); }) &&
                  throws([&] {
                    const auto g = ll::graph_of(lattice(3, 100, 10), o);
                    const ll::Lifelines L(g, {{0, 0, 1}}, {{200, 200, 1}});
                    L.loss({1, 1});
                  }));
  }
  return s.done();
}
