// Checks of allocation/incidence: who finally gains.
//
// - Capitalisation: equal elasticities capitalise half; ε_mig in [0.5, 2] against ε_supply = 1 gives κ in [1/3, 2/3], and κ at
//   every point of a grid over the box lies in the interval; renters, owner-occupiers and landlords together hold the whole value
//   at either end.
// - Propagation: 100 re-spent at c = 0.8 and kept at r = 0.5 gives rounds 100, 40, 16, a total of 166⅔, LM3 1.56, and the
//   total is its own fixed point g + c r total; the survey's LM3 example (100, 80, 40) is 2.2; a series that does not leak is
//   refused.
// - Return to generators: two generators sending 10 each to a platform, which returns 3 and 8: return ratios 0.3 and 0.8, pooled
//   0.55, the platform capturing 9; a pure cycle circulates wholly and a one-way flow not at all, read by economics's Hodge
//   decomposition; flows of two kinds read apart.
// - Floor and ceiling: shortfall and overshoot counted apart, never netted.
// - Targeting: a place given 100 with half its people meant, a place given nothing with 20 meant: half the amount reaches them,
//   and 20 are missed.

#include <allocation/incidence/incidence.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace allocation::incidence;

static bool close(double a, double b, double tol = 1e-9) { return std::abs(a - b) <= tol * std::max(1.0, std::abs(b)); }

int main() {
  calx::verification::Suite s;

  {
    Capitalisation c;
    const auto even = capitalise(100, 0.6, 0.5, c);
    s.require("equal elasticities capitalise half the value", close(even.share.lo, 0.5) && close(even.share.hi, 0.5));
    s.require("renters keep half of their 60, owner-occupiers all of their 40, landlords 30 split 15 and 15",
              close(even.renters.lo, 30) && close(even.owner_occupiers, 40) && close(even.landlords_local.lo, 15) && close(even.landlords_away.lo, 15));
    c.migration = {0.5, 2};
    c.supply = {1, 1};
    const auto box = capitalise(100, 0.6, 0.25, c);
    s.require("ε_mig in [0.5, 2] against ε_supply = 1 gives κ in [1/3, 2/3]", close(box.share.lo, 1.0 / 3) && close(box.share.hi, 2.0 / 3));
    c.supply = {0.5, 3};
    const auto wide = capitalise(100, 0.6, 0.25, c);
    bool inside = true;
    for (int i = 0; i <= 20; i++)
      for (int j = 0; j <= 20; j++) {
        const double m = 0.5 + 1.5 * i / 20, sup = 0.5 + 2.5 * j / 20, k = m / (m + sup);
        inside = inside && k >= wide.share.lo - 1e-12 && k <= wide.share.hi + 1e-12;
      }
    s.require("κ at every point of a grid over the box of elasticities lies in the interval reported", inside);
    const double lo_sum = wide.renters.hi + wide.owner_occupiers + wide.landlords_local.lo + wide.landlords_away.lo;
    const double hi_sum = wide.renters.lo + wide.owner_occupiers + wide.landlords_local.hi + wide.landlords_away.hi;
    s.require("renters, owner-occupiers and landlords together hold the whole value at either end of κ", close(lo_sum, 100) && close(hi_sum, 100));
  }

  {
    const auto p = propagate(100, 0.8, 0.5);
    s.require("100 re-spent at c = 0.8, kept at r = 0.5: rounds 100, 40, 16", close(p.rounds[0], 100) && close(p.rounds[1], 40) && close(p.rounds[2], 16));
    s.require("a total of 166⅔, a multiplier of 5/3 and LM3 1.56", close(p.total, 500.0 / 3) && close(p.multiplier, 5.0 / 3) && close(p.lm3, 1.56));
    s.require("the total is its own fixed point g + c r total, and 66⅔ is spent outside", close(p.total, 100 + 0.4 * p.total) && close(p.leaked, 200.0 / 3));
    s.require("the survey's LM3 example: 100, 80 and 40 give 2.2", close(lm3(100, 80, 40), 2.2));
    bool refused = false;
    try {
      propagate(100, 1, 1);
    } catch (const Refusal&) {
      refused = true;
    }
    s.require("a series that never leaks is refused", refused);
  }

  {
    economics::TransferNetwork N;
    N.positions = {"maker A", "maker B", "platform"};
    N.legs = {{0, 2, 1, 10, "money"}, {2, 0, 1, 3, "money"}, {1, 2, 1, 10, "money"}, {2, 1, 1, 8, "money"}};
    const auto G = returns(N, {0, 1});
    s.require("return ratios 0.3 and 0.8, extraction shares 0.7 and 0.2", close(G.ratio[0], 0.3) && close(G.ratio[1], 0.8) && close(G.extraction[0], 0.7) && close(G.extraction[1], 0.2));
    s.require("pooled, 11 of 20 came back; the platform captured 9", close(G.pooled_ratio, 0.55) && G.captured.size() == 1 && G.captured[0].first == 2 && close(G.captured[0].second, 9));
    s.require("part of the flow circulates (the round trips) and part does not", G.circulating > 0 && G.circulating < 1);

    economics::TransferNetwork cycle;
    cycle.positions = {"a", "b", "c"};
    cycle.legs = {{0, 1, 1, 5, "labour"}, {1, 2, 1, 5, "labour"}, {2, 0, 1, 5, "labour"}};
    const auto C = returns(cycle, {0, 1, 2});
    s.require("a pure cycle returns everything and circulates wholly", close(C.pooled_ratio, 1) && close(C.circulating, 1, 1e-9));
    economics::TransferNetwork tree;
    tree.positions = {"a", "b"};
    tree.legs = {{0, 1, 1, 5, "money"}};
    s.require("a one-way flow returns nothing and does not circulate", close(returns(tree, {0}).ratio[0], 0) && close(returns(tree, {0}).circulating, 0, 1e-9));

    economics::TransferNetwork mixed = cycle;
    mixed.legs.push_back({0, 2, 1, 4, "money"});
    const auto Mx = returns(mixed, {0});
    s.require("flows of two kinds are read apart: the labour circulates wholly, the money not at all",
              Mx.circulating_by_kind.size() == 2 && close(Mx.circulating_by_kind[0].second, 1, 1e-9) && close(Mx.circulating_by_kind[1].second, 0, 1e-9));
  }

  {
    const auto d = doughnut({5, 15, 30}, {1, 2, 1}, 10, 20);
    s.require("floor 10, ceiling 20: shortfall 5 for one person, overshoot 10 for one, counted apart",
              close(d.shortfall[0], 5) && close(d.overshoot[2], 10) && close(d.shortfall_total, 5) && close(d.overshoot_total, 10) && close(d.people_below, 1) && close(d.people_above, 1));
  }

  {
    const auto t = targeting({100, 0}, {100, 100}, {50, 20});
    s.require("half the amount reaches the people meant, and the 20 meant where nothing goes are missed", close(t.reaching, 0.5) && close(t.missed, 20) && close(t.per_person_meant, 100.0 / 70));
  }

  return s.done();
}
