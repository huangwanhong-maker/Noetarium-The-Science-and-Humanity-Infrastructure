// Checks of civil_engineering::water: EPANET's example network Net1 read from its input file in SI; its heads, pressures and flows
// at the first instant against EPANET 2.2's own (tests/fixtures/water_net1_t0.txt, made by make_water_fixtures.py); Darcy–Weisbach's
// law with Swamee and Jain's friction factor on one pipe, by its residual; a pressure-driven junction on Wagner's law; a pump that
// cannot lift against its head closes, and a check valve against reverse flow; an isolated junction reported; the age of water down
// a chain, Σ L A / Q; Todini's index by hand; the simulation of Net1's day, its demands following their pattern and its tank by its
// inflow; the criticality of its links.

#include <civil_engineering/water/water.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <fstream>
#include <map>
#include <numbers>
#include <sstream>
#include <string>

using namespace civil_engineering::water;

int main() {
  calx::verification::Suite s;
  const std::string fx = CIVIL_ENGINEERING_FIXTURES;

  // ---- Net1, read and solved, against EPANET ----
  const Network net1 = read_inp(fx + "/Net1.inp");
  s.require("Net1 read: 9 junctions, a reservoir, a tank, 12 pipes, a pump, its pattern and two controls",
            net1.junctions.size() == 9 && net1.reservoirs.size() == 1 && net1.tanks.size() == 1 && net1.pipes.size() == 12 && net1.pumps.size() == 1 &&
                net1.patterns.at("1").size() == 12 && net1.controls.size() == 2);
  s.check("Net1 in SI: pipe 10 is 10,530 ft = 3,209.544 m long and 18 in = 0.4572 m wide; junction 11 draws 150 gpm",
          std::fabs(net1.pipes[0].length - 3209.544) + std::fabs(net1.pipes[0].diameter - 0.4572) + std::fabs(net1.junctions[1].demand - 150 * 6.30901964e-5), 1e-9);
  {
    const auto snap = solve(net1);
    std::map<std::string, std::size_t> node, link;
    for (std::size_t v = 0; v < snap.nodes.size(); v++) node[snap.nodes[v]] = v;
    for (std::size_t e = 0; e < snap.links.size(); e++) link[snap.links[e]] = e;
    std::ifstream in(fx + "/water_net1_t0.txt");
    std::string line;
    double head = 0, pressure = 0, flow = 0;
    std::size_t nodes = 0, links = 0;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream ls(line);
      std::string kind, id;
      ls >> kind >> id;
      if (kind == "node") {
        double h, p, d;
        ls >> h >> p >> d;
        head = std::max(head, std::fabs(snap.head[node.at(id)] - h));
        pressure = std::max(pressure, std::fabs(snap.pressure[node.at(id)] - p));
        nodes++;
      } else {
        double q;
        ls >> q;
        flow = std::max(flow, std::fabs(snap.flow[link.at(id)] - q));
        links++;
      }
    }
    s.require("Net1 against EPANET: every node and link compared", nodes == 11 && links == 13 && snap.converged);
    s.check("Net1 against EPANET 2.2: heads within a millimetre", head, 1e-3);
    s.check("Net1 against EPANET 2.2: pressures within a millimetre", pressure, 1e-3);
    s.check("Net1 against EPANET 2.2: flows within 10⁻⁶ m³/s (EPANET writes single precision)", flow, 1e-6);
    s.check("Net1: continuity at every junction, and the energy residual at rounding", snap.balance + snap.residual, 1e-9);
  }
  // ---- Darcy–Weisbach on one pipe ----
  {
    Network n;
    n.headloss = "D-W";
    n.reservoirs = {{"R", 100, ""}};
    n.junctions = {{"J", 0, 0, ""}};
    n.tanks = {{"T", 60, 10, 0, 20, 10}};  // head 70
    n.pipes = {{"p", "R", "T", 1000, 0.3, 0.00015, 2.0}};
    const auto sn = solve(n);
    const double q = sn.flow[0], A = std::numbers::pi * 0.09 / 4, v = q / A, nu = Options{}.viscosity, g = Options{}.gravity, Re = v * 0.3 / nu;
    const double t = std::log10(0.00015 / (3.7 * 0.3) + 5.74 / std::pow(Re, 0.9)), f = 0.25 / (t * t);
    s.check("Darcy–Weisbach: 30 m lost along 1 km at Swamee and Jain's friction factor, with a minor loss of K = 2", std::fabs((f * 1000 / 0.3 + 2) * v * v / (2 * g) - 30), 1e-8);
    s.require("Darcy–Weisbach: the flow is turbulent (Re > 4000)", Re > 4000);
  }
  // ---- pressure-driven demand on Wagner's law ----
  {
    Network n;
    n.reservoirs = {{"R", 40, ""}};
    n.junctions = {{"J", 10, 0.05, ""}};
    n.pipes = {{"p", "R", "J", 2000, 0.15, 120, 0}};
    Options o;
    o.pressure_driven = true, o.minimum_pressure = 0, o.required_pressure = 30;
    const auto sn = solve(n, o);
    const double p = sn.pressure[0];
    s.check("pressure-driven: the junction draws q_req √(p/p_req) at the pressure it is left with", std::fabs(sn.demand[0] / (0.05 * std::sqrt(p / 30)) - 1), 2e-4);
    s.require("pressure-driven: it draws less than it asks", sn.demand[0] < 0.05 && p < 30);
    const auto dd = solve(n);
    s.require("demand-driven: the same junction draws all it asks, at a lower pressure", std::fabs(dd.demand[0] - 0.05) < 1e-12 && dd.pressure[0] < p);
  }
  // ---- a pump that cannot lift; a check valve; an isolated junction ----
  {
    Network n;
    n.reservoirs = {{"low", 10, ""}, {"high", 100, ""}};
    n.junctions = {{"J", 0, 0, ""}};
    n.pipes = {{"p", "J", "high", 100, 0.3, 120, 0}};
    n.pumps = {{"pump", "low", "J", {{0.05, 40}}, 0, true, ""}};  // shutoff 53.3 m: it cannot lift 10 m to 100 m
    const auto sn = solve(n);
    s.require("a pump against more head than its shutoff closes, and carries nothing", !sn.open[1] && sn.flow[1] == 0);
    Network c;
    c.reservoirs = {{"A", 50, ""}, {"B", 60, ""}};
    c.junctions = {{"J", 0, 0, ""}};
    c.pipes = {{"p1", "A", "J", 100, 0.2, 120, 0, true, true}, {"p2", "J", "B", 100, 0.2, 120, 0}};
    const auto cv = solve(c);
    s.require("a check valve closes against the reverse flow from B to A", !cv.open[0] && std::fabs(cv.flow[1]) < 1e-12);
    Network i;
    i.reservoirs = {{"R", 50, ""}};
    i.junctions = {{"J1", 0, 0.01, ""}, {"J2", 0, 0.01, ""}};
    i.pipes = {{"p1", "R", "J1", 100, 0.2, 120, 0}, {"p2", "J1", "J2", 100, 0.2, 120, 0, false}};
    const auto is = solve(i);
    s.require("a junction behind a closed pipe is isolated, with no head, and draws nothing", is.isolated.size() == 1 && is.isolated[0] == "J2" && std::isnan(is.head[1]) && is.demand[1] == 0);
  }
  // ---- the age of water down a chain; Todini's index by hand ----
  {
    Network n;
    n.reservoirs = {{"R", 60, ""}};
    n.junctions = {{"A", 0, 0.01, ""}, {"B", 0, 0.02, ""}};
    n.pipes = {{"p1", "R", "A", 1000, 0.2, 120, 0}, {"p2", "A", "B", 500, 0.15, 120, 0}};
    const auto sn = solve(n);
    const auto a = age(n, sn);
    const double t1 = 1000 * std::numbers::pi * 0.04 / 4 / 0.03, t2 = 500 * std::numbers::pi * 0.0225 / 4 / 0.02;
    s.check("water age down a chain: L A / Q along each pipe, summed (A at t₁, B at t₁ + t₂)", std::fabs(a[0] - t1) + std::fabs(a[1] - (t1 + t2)), 1e-6);
    const auto r = resilience(n, sn, 20);
    const double surplus = 0.01 * (sn.head[0] - 20) + 0.02 * (sn.head[1] - 20), avail = 0.03 * 60 - (0.01 * 20 + 0.02 * 20);
    s.check("Todini's index by hand: Σ q (h − h*) / (Q_R H_R − Σ q h*)", std::fabs(r.index - surplus / avail), 1e-12);
  }
  // ---- Net1's day ----
  {
    const auto day = simulate(net1);
    s.require("Net1's day: 25 instants, an hour apart", day.times.size() == 25 && day.times[1] == 3600);
    const double asked0 = day.snapshots[0].required[1], asked2 = day.snapshots[2].required[1];
    s.check("Net1's day: at 2 h junction 11 asks its demand times the pattern's 1.2", std::fabs(asked2 / asked0 - 1.2), 1e-12);
    double worst = 0;
    const double area = net1.tanks[0].area();
    for (std::size_t k = 0; k + 1 < day.times.size(); k++) {
      const double inflow = day.snapshots[k].demand[net1.junctions.size() + 1], before = day.tank_level.at("2")[k], after = day.tank_level.at("2")[k + 1];
      const double expected = std::clamp(before + inflow * 3600 / area, net1.tanks[0].min_level, net1.tanks[0].max_level);
      worst = std::max(worst, std::fabs(after - expected));
    }
    s.check("Net1's day: the tank's level moves by its net inflow each hour", worst, 1e-12);
    bool converged = true;
    for (const auto& sn : day.snapshots) converged = converged && sn.converged;
    s.require("Net1's day: every hour converges", converged);
  }
  // ---- criticality: a loop that can lose a pipe, and a branch that cannot ----
  {
    Network n;
    n.reservoirs = {{"R", 80, ""}};
    n.junctions = {{"A", 0, 0.01, ""}, {"B", 0, 0.01, ""}, {"C", 0, 0.01, ""}, {"D", 0, 0.02, ""}};
    n.pipes = {{"p1", "R", "A", 500, 0.3, 120, 0}, {"p2", "A", "B", 500, 0.25, 120, 0}, {"p3", "A", "C", 500, 0.25, 120, 0},
               {"p4", "C", "B", 500, 0.2, 120, 0}, {"p5", "B", "D", 300, 0.2, 120, 0}};
    const auto crit = criticality(n, Options{}, {"p2", "p3", "p4", "p5"});
    s.check("criticality: losing the branch p5 cuts off D, so 0.03 of 0.05 is delivered", std::fabs(crit.front().delivered - 0.6) + (crit.front().link == "p5" ? 0.0 : 1.0), 1e-4);
    s.require("criticality: D alone is left below the required pressure", crit.front().below == 1);
    s.check("criticality: a pipe of the loop can be lost with all still delivered", std::fabs(crit.back().delivered - 1), 1e-4);
    // Beyond D, two junctions that draw nothing: losing p6 cuts off an island of them alone, joined by a pipe that carries nothing (on
    // EPANET's Net3, a pump closing behind a dead end made such an island, and its equations singular in rounding).
    Network m = n;
    m.junctions.push_back({"E", 0, 0, ""});
    m.junctions.push_back({"F", 0, 0, ""});
    m.pipes.push_back({"p6", "D", "E", 200, 0.15, 120, 0});
    m.pipes.push_back({"p7", "E", "F", 200, 0.15, 120, 0});
    const auto island = criticality(m, Options{}, {"p6"});
    s.check("criticality: an island of junctions that draw nothing, joined by a pipe that carries nothing, is solved, and nothing is lost", std::fabs(island.front().delivered - 1), 1e-4);
  }
  return s.done();
}
