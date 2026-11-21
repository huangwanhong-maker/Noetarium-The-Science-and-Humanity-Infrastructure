// Checks of economics/capture on invented parcels and a project, each answer by hand or by enumeration: a levy of the lesser of cost
// with its mark-up and a share of the uplift, apportioned by uplift or by area; development rights by the formula and at auction with
// its stock identity; tax increment financing and what it diverts from each overlapping jurisdiction when only part of the increment is
// the project's; a developer obligation's incidence; cost recovery and capture of the uplift kept apart; a parcel charged twice over
// named; land readjustment's area identity, contribution ratio, value kept or not, cap and feasibility; and replotting by the
// least total distance, equal to enumeration, its settlements summing to zero.

#include <economics/capture/capture.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>

using namespace economics::capture;

int main() {
  calx::verification::Suite s;
  const std::vector<Parcel> ps = {{"A", 400, 1000, 1100, 20}, {"B", 200, 600, 650, 10}, {"C", 400, 800, 800, 20}, {"D", 100, 300, 290, 10}};
  {
    // Uplift 100, 50, 0 and −10 (taken as none): 150 in all. min(120 · 1.1, 0.5 · 150) = 75.
    const auto L = levy(ps, 120, 0.1, 0.5, Basis::uplift);
    double total = 0;
    for (const auto& p : L) total += p.amount;
    s.check("a levy of min(cost · 1.1, half the uplift) = 75", total - 75, 1e-12);
    s.check("... apportioned by uplift: A 50, B 25", L[0].amount + L[1].amount - 75 + std::fabs(L[0].amount - 50), 1e-12);
    const auto byarea = levy(ps, 120, 0.1, 0.5, Basis::area, 0, 3);
    s.check("... by area in three instalments: A pays 75 · 400/1100 over three years", byarea[0].amount * 3 - 75 * 400.0 / 1100, 1e-12);
    s.require("... a cost-based levy is the lesser when the cost is low: min(40 · 1.1, 75) = 44", std::fabs([&] {
                double t = 0;
                for (const auto& p : levy(ps, 40, 0.1, 0.5, Basis::uplift)) t += p.amount;
                return t;
              }() - 44) < 1e-12);
  }
  s.check("development rights by the formula: 1,000 m² over a basic ratio of 2 at 300 a m² of land, factor 0.6 = 90,000", rights_by_formula(1000, 2, 300, 0.6).amount - 90000, 1e-9);
  {
    const auto a = rights_by_auction(10, 100, {150, 140, 130, 120, 110, 90});
    s.require("an auction of ten certificates at a minimum of 100: five bids at or above it, sold at the minimum, five unsold; authorised = sold + unsold",
              a.sold == 5 && a.unsold == 5 && a.price == 100 && a.revenue == 500 && a.sold + a.unsold == a.authorised);
    const auto b = rights_by_auction(3, 100, {150, 140, 130, 120});
    s.require("... three certificates taken by higher bids: cleared at the lowest accepted, 130", b.sold == 3 && b.price == 130 && b.revenue == 390);
  }
  {
    // Base 100; values 110, 130, 160; the city's rate 1%, the schools' 2%; the project's share of the increment 0.6; no discounting.
    const auto t = tax_increment({110, 130, 160}, 100, 1.0, {{"city", 0.01}, {"schools", 0.02}}, 0.6, 0.0);
    s.check("tax increment: 3% of the increments 10, 30 and 60, three in all", t.pv_district - 3.0, 1e-12);
    s.check("... of which the 40% that would have come without the project diverts 1.2 from the jurisdictions", t.pv_diverted - 1.2, 1e-12);
    double schools = 0;
    for (const auto& p : t.diverted)
      if (p.payer == "jurisdiction:schools") schools += p.amount;
    s.check("... the schools' rate twice the city's, so two thirds of it is theirs", schools - 0.8, 1e-12);
  }
  const auto ob = developer_obligation(100, 50, 0.6, 0.4);
  s.require("a developer obligation of 50 on 100 units: 5,000, 3,000 borne by land and 2,000 by buyers", ob.payments[0].amount == 5000 && ob.on_land == 3000 && ob.on_buyers == 2000);
  {
    const auto L = levy(ps, 120, 0.1, 0.5, Basis::uplift);
    const auto c = compare("levy", L, 0.0, 120, 150, 0.5);
    s.check("the levy recovers 75 of a cost of 120 (ρ = 0.625)", c.cost_recovery - 0.625, 1e-12);
    s.check("... and takes half the uplift (κ = 0.5), beside the statutory 0.5 declared", c.capture - 0.5, 1e-12);
    const auto o = compare("obligation", ob.payments, 0.0, 120, 150);
    s.require("a fee on new floor area recovers cost without being taken from the uplift (κ = 0)", o.capture == 0 && o.cost_recovery > 0);
    const std::vector<Payment> rights{{0, 60, "developers", "A", true}};
    const auto dc = double_charges(ps, {L, rights}, 0.0);
    s.require("a parcel whose uplift of 100 is taken twice over (50 by the levy, 60 for rights) is named", dc.size() == 1 && dc[0].parcel == "A" && dc[0].taken == 110);
  }
  {
    // M 200, A 1,000, N 400 (P 200), E 650, R 150: 1,200 = 400 + 650 + 150; d = 0.35.
    const Areas m{200, 1000, 400, 650, 150};
    const auto r = readjust(m, 100, 150, 0.30, 0.05, {}, {{1, 20000, "the project", "", false}}, 2, 0.05);
    s.require("land readjustment: the areas balance and the contribution ratio is (200 + 150)/1000 = 0.35",
              r.areas_balance && std::fabs(r.contribution - 0.35) < 1e-12 && std::fabs(r.public_share - 0.2) < 1e-12 && std::fabs(r.reserve_share - 0.15) < 1e-12);
    s.require("... with values rising by half, (1 − 0.35) · 1.5 = 0.975 < 1: the owners' land does not keep its value; and the cap of 0.3 is exceeded",
              std::fabs(r.kept - 0.975) < 1e-12 && !r.value_kept && !r.within_cap);
    const auto q = readjust(m, 100, 160, 0.40, 0.05, {}, {{1, 20000, "the project", "", false}}, 2, 0.05);
    s.check("... rising to 160, (0.65)(1.6) = 1.04: kept; the reserve sold for 150 · 160 · 0.95 = 22,800", q.reserve_revenue - 22800, 1e-9);
    s.check("... feasible: 22,800/1.05² − 20,000/1.05", q.pv_balance - (22800 / 1.1025 - 20000 / 1.05), 1e-9);
    s.require("... value kept, within a cap of 0.4", q.value_kept && q.within_cap && q.feasible);
    const Areas bad{200, 1000, 400, 600, 150};
    s.require("areas that do not balance are reported: 50 m² unaccounted", std::fabs(readjust(bad, 100, 160, none, 0, {}, {}, 0, 0).imbalance - 50) < 1e-9);
  }
  {
    std::vector<Parcel> owners = {{"o1", 100, 1000, 0, 0, 0, 0}, {"o2", 100, 500, 0, 0, 10, 0}, {"o3", 100, 500, 0, 0, 0, 10}};
    const std::vector<Plot> plots = {{"p1", 60, 20, 9, 1}, {"p2", 60, 20, 1, 1}, {"p3", 50, 20, 1, 9}, {"p4", 80, 20, 20, 20}};
    const auto r = replot(owners, plots);
    // By enumeration over every injection of three owners into four plots.
    double best = INFINITY;
    std::vector<std::size_t> idx = {0, 1, 2, 3};
    do {
      double d = 0;
      for (std::size_t i = 0; i < 3; i++) d += std::hypot(owners[i].x - plots[idx[i]].x, owners[i].y - plots[idx[i]].y);
      best = std::min(best, d);
    } while (std::next_permutation(idx.begin(), idx.end()));
    s.check("replotting by the least total distance equals enumeration: 3√2", r.distance - best + std::fabs(best - 3 * std::sqrt(2.0)), 1e-9);
    s.require("... each owner given the nearest plot here: o1 the plot at (1, 1), o2 at (9, 1), o3 at (1, 9)", r.plot_of[0] == 1 && r.plot_of[1] == 0 && r.plot_of[2] == 2);
    const double settled = std::accumulate(r.settlement.begin(), r.settlement.end(), 0.0);
    s.check("... entitlements in proportion to the values before (o1 half of 3,400), and the settlements sum to zero",
            std::fabs(r.entitlement[0] - 1700) + std::fabs(settled), 1e-9);
  }
  return s.done();
}
