// Checks of industrial_ecology::lca on Heijungs and Suh's example (2002, §2.2: electricity from fuel,
// fuel refined with electricity), by hand: the scaling, the inventory and the contributions; a process
// with two products, partitioned and expanded, by hand; the elasticities against differences, and their
// sum over each process's own coefficients, which is zero (rescaling a process changes nothing); Monte
// Carlo's spread; the structural paths, which follow the one cycle of the system and sum to the total.

#include <industrial_ecology/lca/lca.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace industrial_ecology::lca;

namespace {

// Power plant: 10 kWh out, 2 L of fuel in, 1 kg of CO₂. Refinery: 100 L out, 50 kWh in, 10 kg of CO₂.
ProductSystem example() {
  ProductSystem ps;
  ps.products = {"electricity (kWh)", "fuel (L)"};
  ps.processes = {"power plant", "refinery"};
  ps.interventions = {"CO2 (kg)", "SO2 (kg)"};
  ps.A = Rectangular::from_rows({{10, -50}, {-2, 100}});
  ps.B = Rectangular::from_rows({{1, 10}, {0.1, 2}});
  return ps;
}
Method gwp_and_acidification() {
  Method m;
  m.categories = {"climate change", "acidification"};
  m.units = {"kg CO2-eq", "kg SO2-eq"};
  m.C = Rectangular::from_rows({{1, 0}, {0, 1}});
  return m;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto ps = example();
  const auto m = gwp_and_acidification();
  const std::vector<double> f{1000, 0};

  // ---- the example by hand ----
  {
    const auto R = assess(ps, m, f);
    // det A = 900, A⁻¹ = [[100, 50], [2, 10]] / 900: s = (1000/9, 20/9).
    s.check("Heijungs and Suh: the scaling s = A⁻¹ f = (111.1, 2.222)", std::fabs(R.scaling[0] - 1000.0 / 9) + std::fabs(R.scaling[1] - 20.0 / 9), 1e-12);
    s.check("Heijungs and Suh: 1000 kWh emit 133.3 kg of CO₂", std::fabs(R.impacts[0] - 1200.0 / 9), 1e-12);
    s.check("the contributions of the processes sum to the impact", std::fabs(R.contribution(0, 0) + R.contribution(0, 1) - R.impacts[0]) + std::fabs(R.contribution(1, 0) + R.contribution(1, 1) - R.impacts[1]), 1e-12);
  }
  // ---- a process with two products ----
  {
    // A combined heat and power plant yields 10 kWh and 20 MJ of heat from 3 L of fuel, emitting 2 kg of CO₂.
    ProductSystem two;
    two.products = {"electricity (kWh)", "heat (MJ)", "fuel (L)"};
    two.processes = {"CHP", "refinery"};
    two.interventions = {"CO2 (kg)"};
    two.A = Rectangular::from_rows({{10, 0}, {20, 0}, {-3, 100}});
    two.B = Rectangular::from_rows({{2, 0}});
    Method c;
    c.categories = {"climate change"};
    c.C = Rectangular::from_rows({{1}});
    // Partition by energy: 10 kWh = 36 MJ against 20 MJ, so 36/56 to electricity.
    std::vector<double> factors{36.0 / 56, 20.0 / 56, 0};
    const auto part = partition(two, 0, factors);
    s.require("partition: CHP becomes two processes, one per product, and the system is square", part.square() && part.processes.size() == 3);
    // With a boiler beside it, the CHP's heat can instead displace the boiler's.
    ProductSystem chp;
    chp.products = two.products;
    chp.processes = {"CHP", "boiler", "refinery"};
    chp.interventions = two.interventions;
    chp.A = Rectangular::from_rows({{10, 0, 0}, {20, 50, 0}, {-3, -5, 100}});
    chp.B = Rectangular::from_rows({{2, 1.5, 0}});
    // 1 kWh by the partitioned CHP: 1/10 of its electricity share, 2 kg × 36/56 / 10.
    const auto P = assess(part, c, {1, 0, 0});
    s.check("partition by energy: 1 kWh carries (36/56) · 2 kg / 10 = 0.1286 kg", std::fabs(P.impacts[0] - 2.0 * 36 / 56 / 10), 1e-14);
    // Substitution: the CHP's heat displaces the boiler's, 20 MJ = 0.4 of the boiler's 50 MJ.
    const auto sub = substitute(chp, 0, 1, 1);
    // The heat row of the CHP vanishes, its CO₂ credited 0.4 · 1.5 and its fuel 0.4 · 5 L.
    s.check("substitution: the CHP's column is credited with 0.4 of the boiler's", std::fabs(sub.B(0, 0) - (2 - 0.6)) + std::fabs(sub.A(2, 0) - (-3 + 2)) + std::fabs(sub.A(1, 0)), 1e-14);
  }
  // ---- elasticities ----
  {
    const auto R = assess(ps, m, f);
    const auto E = elasticities(ps, m, R, 0);
    double worst = 0;
    for (const auto& e : E) {
      ProductSystem q = ps;
      const double h = 1e-6 * std::fabs(e.matrix == 'A' ? ps.A(e.row, e.col) : ps.B(e.row, e.col));
      (e.matrix == 'A' ? q.A(e.row, e.col) : q.B(e.row, e.col)) += h;
      const double up = assess(q, m, f).impacts[0];
      (e.matrix == 'A' ? q.A(e.row, e.col) : q.B(e.row, e.col)) -= 2 * h;
      const double down = assess(q, m, f).impacts[0];
      worst = std::max(worst, std::fabs((up - down) / (2 * h) - e.derivative) / (1 + std::fabs(e.derivative)));
    }
    s.check("elasticities: every derivative agrees with central differences", worst, 1e-6);
    double sums[2] = {0, 0};
    for (const auto& e : E) sums[e.col] += e.elasticity;
    s.check("elasticities: over each process's own coefficients they sum to zero (rescaling a process is no change)", std::fabs(sums[0]) + std::fabs(sums[1]), 1e-12);
  }
  // ---- Monte Carlo ----
  {
    const auto none = monte_carlo(ps, m, f, 1.0, 1.0, 50);
    s.check("Monte Carlo: with no spread every draw is the deterministic result", std::fabs(none.lower[0] - 1200.0 / 9) + std::fabs(none.upper[0] - 1200.0 / 9), 1e-10);
    const auto some = monte_carlo(ps, m, f, 1.1, 1.2, 2000, 7);
    s.require("Monte Carlo: the 95% interval holds the deterministic result and has width", some.lower[0] < 1200.0 / 9 && 1200.0 / 9 < some.upper[0] && some.upper[0] - some.lower[0] > 10);
  }
  // ---- structural paths ----
  {
    const auto W = paths(ps, m, f, 0, 1e-12, 60, 100);
    s.check("paths: their total is the impact", std::fabs(W.total - 1200.0 / 9), 1e-12);
    s.check("paths: the one cycle (electricity ⇄ fuel) is followed until what is left is below 10⁻¹⁰", (W.total - W.covered) / W.total, 1e-10);
    // The direct emission of 1000 kWh: 1000 · (1 kg / 10 kWh) = 100 kg; then the fuel for it: 1000 · 0.2 L · 0.1 kg/L = 20 kg.
    s.require("paths: the direct emission first (100 kg), then the fuel's (20 kg)",
              W.walks.size() >= 2 && std::fabs(W.walks[0].value - 100) < 1e-12 && std::fabs(W.walks[1].value - 20) < 1e-12 && W.walks[1].nodes.size() == 2);
  }

  return s.done();
}
