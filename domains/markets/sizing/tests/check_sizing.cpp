// Checks of the size of a market (sizing.hpp).
//
// - The bounds of a tree of sums and products are attained at the leaves' lows and highs: against every corner of the leaves' ranges.
// - Elasticities: 1 for every leaf of a chain of products; adding to 1 over a sum of leaves; equal to finite differences on a graph
//   in which nodes are used more than once.
// - A swing is the size with one leaf at its ends; for a chain of products its ends are in the ratio of the leaf's.
// - TAM ≥ SAM ≥ SOM; a worked chain: 120 k buyers × 30% × 240 a year = 8.64 M, × 50% = 4.32 M, × 8% = 345.6 k.
// - Two estimates agree when their ranges meet; else the gap is the ratio of the nearer ends.
// - Buyers of a later year: with a rate of 1 they are the population; in a stable population they grow by λ a step.
// - Derived demand: a sector's row of the table adds to its output; under the table's own final demand the model gives the table's
//   deliveries; it is linear in the final demand.
// - What is not a range or a tree is refused.

#include <markets/sizing/sizing.hpp>

#include <demography/fertility/fertility.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace ms = markets::sizing;
namespace dp = demography::projection;
namespace lt = demography::lifetable;

namespace {

ms::Range range(calx::random::Stream& rng) {
  const double low = rng.uniform(0.2, 2), mid = low * rng.uniform(1, 1.6);
  return {low, mid, mid * rng.uniform(1, 1.6)};
}

template <typename F>
bool refuses(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(2026, 11, 11, 2);

  // ---- bounds, against every corner ----
  {
    // (a b + c) (a + d e) + b: a and b are used twice.
    ms::Tree T;
    const auto a = T.leaf("a", range(rng)), b = T.leaf("b", range(rng)), c = T.leaf("c", range(rng)), d = T.leaf("d", range(rng)), e = T.leaf("e", range(rng));
    const auto ab = T.product("a b", {a, b}), left = T.sum("a b + c", {ab, c}), de = T.product("d e", {d, e}), right = T.sum("a + d e", {a, de});
    const auto both = T.product("both", {left, right});
    T.sum("size", {both, b});
    const auto R = ms::evaluate(T);
    double least = INFINITY, most = 0;
    for (unsigned corner = 0; corner < 32; corner++) {
      const double y = ms::values(T, [&](std::size_t i, const ms::Range& r) { return corner >> i & 1 ? r.high : r.low; })[T.root()];
      least = std::min(least, y), most = std::max(most, y);
    }
    s.check("the low of a tree of sums and products is its least value over every corner of the leaves' ranges", R[T.root()].low / least - 1, 1e-14);
    s.check("... and its high the greatest", R[T.root()].high / most - 1, 1e-14);
    const auto x = [&](std::size_t i) { return T.nodes[i].value.mid; };
    s.check("... and its mid the value at the mids", R[T.root()].mid / ((x(a) * x(b) + x(c)) * (x(a) + x(d) * x(e)) + x(b)) - 1, 1e-14);

    // Elasticities against finite differences.
    const auto eps = ms::elasticities(T);
    double worst = 0;
    for (std::size_t l : {a, b, c, d, e}) {
      const double h = 1e-6;
      const auto at = [&](double f) { return ms::values(T, [&](std::size_t i, const ms::Range& r) { return i == l ? r.mid * f : r.mid; })[T.root()]; };
      worst = std::max(worst, std::fabs(eps[l] - (std::log(at(1 + h)) - std::log(at(1 - h))) / (std::log1p(h) - std::log1p(-h))));
    }
    s.check("elasticities on a graph whose nodes are used twice are the finite differences of ln size by ln leaf", worst, 1e-9);
    s.require("... and are zero for what is not a leaf", eps[ab] == 0 && eps[both] == 0 && eps[T.root()] == 0);

    // Swings.
    const auto sw = ms::swings(T);
    bool ordered = sw.size() == 5, ends = true;
    for (std::size_t k = 0; k + 1 < sw.size(); k++) ordered = ordered && sw[k].width() >= sw[k + 1].width();
    for (const auto& w : sw) ends = ends && w.low <= R[T.root()].mid && R[T.root()].mid <= w.high && w.low >= R[T.root()].low - 1e-12 && w.high <= R[T.root()].high + 1e-12;
    s.require("swings come widest first, each about the mid and within the bounds", ordered && ends);
  }
  {
    ms::Tree chain, total;
    const auto buyers = chain.leaf("buyers", {100e3, 120e3, 140e3}, "a census", false), need = chain.leaf("need", {0.2, 0.3, 0.4}), spend = chain.leaf("spend", {180, 240, 320});
    chain.product("TAM", {buyers, need, spend});
    const auto e = ms::elasticities(chain);
    s.check("every leaf of a chain of products has elasticity 1", std::fabs(e[buyers] - 1) + std::fabs(e[need] - 1) + std::fabs(e[spend] - 1), 1e-14);
    const auto sw = ms::swings(chain);
    double ratio = 0;
    for (const auto& w : sw) ratio = std::max(ratio, std::fabs(w.high / w.low - chain.nodes[w.leaf].value.high / chain.nodes[w.leaf].value.low));
    s.check("... and its swing's ends are in the ratio of the leaf's", ratio, 1e-13);
    s.require("... the widest swing that of the leaf with the widest range in proportion", sw[0].leaf == need);
    const auto north = total.leaf("north", ms::exactly(30)), south = total.leaf("south", ms::exactly(50)), west = total.leaf("west", ms::exactly(20));
    total.sum("all", {north, south, west});
    const auto t = ms::elasticities(total);
    s.check("the elasticities of the leaves of a sum add to 1, each its part of the sum", std::fabs(t[north] + t[south] + t[west] - 1) + std::fabs(t[south] - 0.5), 1e-14);
    s.require("a datum says where it comes from, an assumption that it is one", !chain.nodes[buyers].assumed && chain.nodes[buyers].source == "a census" && chain.nodes[need].assumed);
  }

  // ---- TAM, SAM, SOM ----
  {
    const auto stages = ms::chain({100e3 * 0.2 * 180, 120e3 * 0.3 * 240, 140e3 * 0.4 * 320}, {{0.35, 0.5, 0.6}, {0.04, 0.08, 0.12}});
    s.check("120 k buyers × 30% with the need × 240 a year: all who could buy, 8.64 M", stages[0].mid / 8.64e6 - 1, 1e-14);
    s.check("... × 50% served: 4.32 M", stages[1].mid / 4.32e6 - 1, 1e-14);
    s.check("... × 8% won: 345.6 k", stages[2].mid / 345.6e3 - 1, 1e-14);
    s.check("... its low 100 k × 20% × 180 × 35% × 4% = 50.4 k", stages[2].low / 50.4e3 - 1, 1e-14);
    s.check("... its high 140 k × 40% × 320 × 60% × 12% = 1.29 M", stages[2].high / 1290240.0 - 1, 1e-14);
    bool falls = true;
    for (std::size_t k = 0; k + 1 < stages.size(); k++) falls = falls && stages[k + 1].low <= stages[k].low && stages[k + 1].mid <= stages[k].mid && stages[k + 1].high <= stages[k].high;
    s.require("TAM ≥ SAM ≥ SOM at the lows, the mids and the highs", falls);
  }

  // ---- two estimates ----
  {
    const auto agree = ms::reconcile({3, 5, 8}, {6, 9, 12}), apart = ms::reconcile({3, 5, 8}, {12, 15, 20});
    s.require("two estimates whose ranges meet agree on what both allow", agree.agree && agree.low == 6 && agree.high == 8);
    s.check("two whose ranges do not meet differ at least by the ratio of the nearer ends", apart.gap - 1.5, 1e-15);
    s.require("... and do not agree", !apart.agree);
  }

  // ---- buyers of a later year ----
  {
    std::vector<double> ages, m;
    for (int a = 0; a <= 90; a += 5) ages.push_back(a), m.push_back(0.0004 + 0.00004 * std::exp(0.092 * (a + 2.5)) + (a == 0 ? 0.002 : 0.0));
    const auto women = lt::life_table(ages, m);
    const std::size_t k = 11;
    std::vector<double> f(k, 0.0);
    const std::vector<double> schedule{0.018, 0.098, 0.158, 0.138, 0.072, 0.018, 0.0023};
    for (std::size_t i = 0; i < schedule.size(); i++) f[3 + i] = schedule[i];
    const auto M = dp::leslie(women, k, f, 0.4886);
    const auto st = dp::stable(M);
    std::vector<double> N(k), rate(k), ones(k, 1.0);
    for (std::size_t i = 0; i < k; i++) N[i] = 1e5 * st.structure[i], rate[i] = i >= 3 ? rng.uniform(0.05, 0.6) : 0.0;
    const auto path = ms::buyers_ahead(M, N, rate, 6);
    double worst = 0;
    for (std::size_t t = 0; t + 1 < path.size(); t++) worst = std::max(worst, std::fabs(path[t + 1] / path[t] - st.lambda));
    s.check("in a stable population the buyers grow by λ a step, whatever the rates of buying by age", worst, 1e-9);
    double total = 0;
    for (double n : N) total += n;
    s.check("with a rate of 1 at every age the buyers are the population", ms::buyers_ahead(M, N, ones, 0)[0] / total - 1, 1e-14);
    // A population all in one group: its buyers follow it up the ages.
    std::vector<double> young(k, 0.0), adults(k, 0.0);
    young[2] = 1000;
    for (std::size_t i = 4; i < k; i++) adults[i] = 1;
    const auto later = ms::buyers_ahead(M, young, adults, 3);
    s.require("those not yet of an age to buy are no buyers now", later[0] == 0 && later[1] == 0);
    s.check("... and are buyers when they reach it, less those who died on the way", later[2] / (1000 * M.survival[2] * M.survival[3]) - 1, 1e-14);
  }

  // ---- derived demand ----
  {
    economics::InputOutputTable T;
    T.sectors = {"farms", "mills", "bakers", "cafes"};
    T.z = calx::linalg::Matrix(4);
    const double z[4][4] = {{5, 40, 2, 3}, {1, 6, 30, 8}, {0, 1, 4, 25}, {2, 3, 1, 2}};
    for (std::size_t i = 0; i < 4; i++)
      for (std::size_t j = 0; j < 4; j++) T.z(i, j) = z[i][j];
    T.final_demand = {30, 15, 50, 92};
    const auto x = T.output();
    T.value_added.resize(4);
    for (std::size_t j = 0; j < 4; j++) {
      double col = 0;
      for (std::size_t i = 0; i < 4; i++) col += T.z(i, j);
      T.value_added[j] = x[j] - col;
    }
    const auto mills = ms::derived_demand(T, 1);
    s.check("what is bought of a sector's product, by the sectors and by final demand, adds to its output", mills.total() / x[1] - 1, 1e-14);
    s.require("... the bakers the mills' largest buyers", mills.by_sector[2] == 30 && mills.final_demand == 15);
    const auto L = economics::leontief(T);
    const auto same = ms::derived_demand(L, 1, T.final_demand);
    double worst = 0;
    for (std::size_t j = 0; j < 4; j++) worst = std::max(worst, std::fabs(same.by_sector[j] - mills.by_sector[j]));
    s.check("under the table's own final demand the model gives the table's deliveries", worst, 1e-11);
    auto more = T.final_demand;
    more[3] *= 1.5;  // the cafes sell half as much again
    const auto after = ms::derived_demand(L, 1, more), twice = ms::derived_demand(L, 1, {60, 30, 100, 184});
    s.require("when the cafes sell more, more is bought of the mills by every sector that the cafes' purchases reach", after.intermediate() > mills.intermediate() && after.by_sector[3] > mills.by_sector[3]);
    s.check("derived demand is linear in the final demand", twice.intermediate() / mills.intermediate() - 2, 1e-12);
  }

  // ---- refusals ----
  {
    s.require("a range out of order, a share above one, a node used before it is made and an empty sum are refused",
              refuses([] { ms::Tree T; T.leaf("x", {2, 1, 3}); }) && refuses([] { ms::chain({1, 2, 3}, {{0.5, 0.9, 1.2}}); })
                  && refuses([] { ms::Tree T; T.sum("s", {3}); }) && refuses([] { ms::Tree T; T.product("p", {}); })
                  && refuses([] { ms::buyers({1, 2}, {0.5}); }));
  }

  return s.done();
}
