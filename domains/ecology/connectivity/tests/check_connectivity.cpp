// Checks of ecology/connectivity: PC and its parts for two patches in closed form (no connector); a chain of three whose middle
// patch carries the best path, its connector part exactly the gain of the path through it; IIC for linked and unlinked
// patches; circuits on a strip, whose effective resistance between its shorted ends is (L − 1)/w with four neighbours and
// (L − 1)/(w + 2(w − 1)/√2) with eight, the current 1/w through every inner cell; least-cost distances in Manhattan and octile
// form; a barrier cutting the patches apart; the effective resistance a metric on a random landscape.

#include <ecology/connectivity/circuits.hpp>
#include <ecology/connectivity/patches.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>
#include <vector>

using namespace ecology::connectivity;

int main() {
  calx::verification::Suite s;
  const double inf = std::numeric_limits<double>::infinity();

  // ---- two patches ---------------------------------------------------------------------------------------
  {
    const std::vector<double> a{2, 3};
    const Distances d{{0, 400}, {400, 0}};
    const double median = 250, AL = 100;
    const double p = std::exp(-std::log(2.0) * 400 / median), num = 4 + 9 + 2 * 6 * p;
    const auto c = connectivity(a, d, median, 500, AL);
    s.check("PC: (a₀² + a₁² + 2 a₀ a₁ p) / A_L²", c.pc - num / (AL * AL), 1e-15);
    s.check("ECA = √(Σ a_i a_j p*_ij)", c.eca - std::sqrt(num), 1e-12);
    s.check("dPC₀ = 100 (num − a₁²)/num", c.importance[0].total - 100 * (num - 9) / num, 1e-10);
    s.check("intra₀ = 100 a₀²/num", c.importance[0].intra - 100 * 4 / num, 1e-10);
    s.check("flux₀ = 100 · 2 a₀ a₁ p / num", c.importance[0].flux - 100 * 12 * p / num, 1e-10);
    s.check("connector₀ = 0 with two patches", c.importance[0].connector, 1e-10);
    s.check("IIC: linked within the threshold, (a₀² + a₁² + a₀a₁)/A_L²", c.iic - (4 + 9 + 6.0) / (AL * AL), 1e-15);
    const auto far = connectivity(a, d, median, 300, AL);
    s.check("IIC: beyond the threshold, (a₀² + a₁²)/A_L²", far.iic - 13 / (AL * AL), 1e-15);
  }

  // ---- a chain of three: the middle patch as a stepping stone -----------------------------------------
  {
    const std::vector<double> a{5, 1, 4};
    const Distances d{{0, 100, 250}, {100, 0, 100}, {250, 100, 0}};
    const double median = 120, k = std::log(2.0) / median;
    const auto c = connectivity(a, d, median, 150, 1000);
    const double p01 = std::exp(-k * 100), via = std::exp(-k * 200), direct = std::exp(-k * 250);
    const double num = 25 + 1 + 16 + 2 * (5 * 1 * p01 + 1 * 4 * p01 + 5 * 4 * via);
    s.check("chain: PC numerator with p*₀₂ through the middle", c.pc * 1e6 - num, 1e-9);
    s.check("chain: the middle's connector part is the gain of the path through it", c.importance[1].connector - 100 * 2 * 20 * (via - direct) / num, 1e-9);
    s.require("chain: the end patches connect nothing", std::fabs(c.importance[0].connector) < 1e-9 && std::fabs(c.importance[2].connector) < 1e-9);
    s.require("chain: dIIC of the middle exceeds its own share (it links the ends)", c.d_iic[1] > 100 * 1.0 / 42);
  }

  // ---- circuits on a strip -------------------------------------------------------------------------------
  {
    const std::size_t L = 9, w = 5;
    ResistanceGrid g;
    g.nx = L, g.ny = w, g.cell = 10, g.resistance.assign(L * w, 1.0), g.eight = false;
    std::vector<std::size_t> left, right;
    for (std::size_t j = 0; j < w; j++) left.push_back(j * L), right.push_back(j * L + L - 1);
    const auto r4 = circuits(g, {left, right});
    s.check("strip, 4 neighbours: R = (L − 1)/w", r4.resistance[0][1] - (L - 1.0) / w, 1e-9);
    double worst = 0;
    for (std::size_t j = 0; j < w; j++)
      for (std::size_t i = 1; i + 1 < L; i++) worst = std::max(worst, std::fabs(r4.current[j * L + i] - 1.0 / w));
    s.check("strip, 4 neighbours: 1/w through every inner cell", worst, 1e-9);
    s.check("strip, 4 neighbours: least cost (L − 1) × side", r4.least_cost[0][1] - (L - 1.0) * 10, 1e-9);
    g.eight = true;
    const auto r8 = circuits(g, {left, right});
    s.check("strip, 8 neighbours: R = (L − 1)/(w + 2(w − 1)/√2)", r8.resistance[0][1] - (L - 1.0) / (w + 2 * (w - 1) / std::numbers::sqrt2), 1e-9);
    s.require("strip: the resistances are symmetric", r8.resistance[0][1] == r8.resistance[1][0]);
  }

  // ---- least cost between single cells, and a barrier -----------------------------------------------------
  {
    ResistanceGrid g;
    g.nx = 6, g.ny = 6, g.cell = 1, g.resistance.assign(36, 1.0), g.eight = false;
    const auto r = circuits(g, {{0}, {4 * 6 + 3}});
    s.check("least cost, 4 neighbours: Manhattan 3 + 4", r.least_cost[0][1] - 7, 1e-12);
    g.eight = true;
    const auto r8 = circuits(g, {{0}, {4 * 6 + 3}});
    s.check("least cost, 8 neighbours: octile 3√2 + 1", r8.least_cost[0][1] - (3 * std::numbers::sqrt2 + 1), 1e-12);
    for (std::size_t j = 0; j < 6; j++) g.resistance[j * 6 + 2] = inf;  // a wall at column 2
    const auto cut = circuits(g, {{0}, {5}});
    s.require("barrier: infinite resistance and cost across a wall", std::isinf(cut.resistance[0][1]) && std::isinf(cut.least_cost[0][1]));
  }

  // ---- a metric on a random landscape --------------------------------------------------------------------
  {
    calx::random::Stream rng(3, 0, 0, 0);
    ResistanceGrid g;
    g.nx = 14, g.ny = 11, g.cell = 5;
    for (std::size_t c = 0; c < g.nx * g.ny; c++) g.resistance.push_back(std::exp(2 * rng.uniform()));
    const auto r = circuits(g, {{0, 1}, {g.nx * 5 + 7}, {g.nx * g.ny - 1}, {g.nx * 10}});
    bool triangle = true, below = true;
    for (std::size_t i = 0; i < 4; i++)
      for (std::size_t j = 0; j < 4; j++)
        for (std::size_t k = 0; k < 4; k++) triangle = triangle && r.resistance[i][j] <= r.resistance[i][k] + r.resistance[k][j] + 1e-9;
    // Rayleigh's monotonicity: one path's series resistance bounds R; the least-cost path is one path, with its own cost /
    // (side × length factor), so R ≤ least cost / side.
    for (std::size_t i = 0; i < 4; i++)
      for (std::size_t j = 0; j < 4; j++) below = below && r.resistance[i][j] <= r.least_cost[i][j] / g.cell + 1e-9;
    s.require("random landscape: effective resistance obeys the triangle inequality", triangle);
    s.require("random landscape: R ≤ the least-cost path's resistance (Rayleigh's monotonicity)", below);
  }

  return s.done();
}
