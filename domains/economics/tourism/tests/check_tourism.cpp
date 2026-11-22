// Checks of economics::tourism, each worked by hand: a two-product table balanced, demand from nights and spending, its effects through
// the Leontief inverse with value added, imported inputs and taxes summing to domestic demand (Proposition 1); a levy passed on in full;
// a levy shared by finite elasticities, the small-levy pass-through, the burdens summing to the revenue and Harberger's triangle; a rate
// in closed form at the equilibrium; the levy that raises most, at P0 / (|ε_d| − 1) or at the cap; refusals.

#include <economics/tourism/tourism.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace economics::tourism;

namespace {
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
Accounts two() {
  Accounts A;
  A.domestic.sectors = {"a", "b"};
  A.domestic.z = calx::linalg::Matrix(2);
  A.domestic.z(0, 0) = 10, A.domestic.z(0, 1) = 20, A.domestic.z(1, 0) = 30, A.domestic.z(1, 1) = 0;
  A.domestic.final_demand = {70, 70};
  A.imported = {10, 20}, A.taxes = {5, 10}, A.gva = {45, 50};
  A.domestic.value_added = {A.imported[0] + A.taxes[0] + A.gva[0], A.imported[1] + A.taxes[1] + A.gva[1]};
  A.domestic_share = {0.8, 1.0};
  return A;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const Accounts A = two();
  s.check("a two-product table: outputs 100 and 100, every column balanced", A.imbalance(), 1e-12);
  const auto d = demand(A, {{"a", {{"DOM", 50}, {"FOR", 100}}, 0.8}}, {{"DOM", 10}, {"FOR", 20}});
  s.check("demand: 10 domestic nights at 50 and 20 foreign at 100, 80% at basic prices: 2,000 of product a", std::fabs(d[0] - 2000) + d[1], 1e-9);
  {
    const auto E = effects(A, {10, 0});
    // L = [[1, 0.2], [0.3, 0.9]] / 0.84; domestic demand (8, 0); output (8, 2.4) / 0.84.
    s.check("10 of product a, 8 of it domestic: output 8/0.84 and 2.4/0.84", std::fabs(E.total_output[0] - 8 / 0.84) + std::fabs(E.total_output[1] - 2.4 / 0.84), 1e-12);
    s.check("... direct value added 0.45 · 8 = 3.6, total 0.45 · 8/0.84 + 0.5 · 2.4/0.84", std::fabs(E.direct_gva - 3.6) + std::fabs(E.total_gva - (0.45 * 8 + 0.5 * 2.4) / 0.84), 1e-12);
    s.check("Proposition 1: total value added, induced imported inputs and induced taxes sum to domestic demand, 8",
            std::fabs(E.total_gva + E.induced_imports + E.induced_taxes - 8), 1e-12);
    Accounts J = A;
    J.jobs = {0.5, 0.2};  // persons a year per unit of output
    const auto EJ = effects(J, {10, 0});
    s.check("jobs (ADR 0161): 0.5 and 0.2 persons a unit of output: 0.5 · 8 directly, 0.5 · 8/0.84 + 0.2 · 2.4/0.84 along the chains",
            std::fabs(EJ.jobs_direct - 4) + std::fabs(EJ.jobs_total - (0.5 * 8 + 0.2 * 2.4) / 0.84), 1e-12);
    s.check("... leakage (2 imported + induced imported inputs) over 10; the tourism ratio of a 8/100",
            std::fabs(E.leakage - (2 + (0.1 * 8 + 0.2 * 2.4) / 0.84) / 10) + std::fabs(E.ratio[0] - 0.08), 1e-12);
  }
  {
    const Market m{1000, 100, -1.5};
    const auto L = flat(m, 5);
    s.check("a levy of 5 passed on in full (supply infinitely elastic): the visitor pays 105, nights 1000 · 1.05^−1.5, revenue 5 times them",
            std::fabs(L.consumer_price - 105) + std::fabs(L.nights_after - 1000 * std::pow(1.05, -1.5)) + std::fabs(L.revenue - 5 * L.nights_after) + std::fabs(L.pass_through - 1), 1e-9);
  }
  {
    const Market m{1000, 100, -1, 1};
    const auto small = flat(m, 0.01);
    s.check("finite elasticities −1 and 1: a small levy's pass-through ε_s/(ε_s − ε_d) = 1/2", std::fabs(small.pass_through - 0.5), 1e-4);
    const auto L = flat(m, 10);
    s.check("... a levy of 10: the visitor's and the operator's prices 10 apart, the burdens summing to the revenue",
            std::fabs(L.consumer_price - L.producer_price - 10) + std::fabs(L.visitors_burden + L.operators_burden - L.revenue), 1e-8);
    s.check("... demand meets supply: 1000 (p_c/100)^−1 = 1000 (p_p/100)^1", std::fabs(1000 * 100 / L.consumer_price - 1000 * L.producer_price / 100), 1e-7);
    const auto h = flat(m, 0.1);
    s.require("the excess burden is not negative, and for a small levy half the levy times the nights lost, within 1%",
              L.excess_burden >= 0 && std::fabs(h.excess_burden - 0.5 * 0.1 * h.nights_lost) < 0.01 * h.excess_burden);
  }
  {
    const Market m{1000, 100, -0.8, 2};
    const auto R = ad_valorem(m, 0.1);
    s.check("a rate of 10% on the operator's price: p_c = 1.1 p_p, and demand meets supply at the closed form",
            std::fabs(R.consumer_price - 1.1 * R.producer_price) + std::fabs(1000 * std::pow(R.consumer_price / 100, -0.8) - 1000 * std::pow(R.producer_price / 100, 2)), 1e-9);
  }
  {
    const auto P = revenue_maximising(Market{1000, 100, -2}, 300);
    s.check("the levy that raises most, passed on in full at ε_d = −2: P0 / (|ε_d| − 1) = 100", std::fabs(P.levy - 100), 1e-4);
    s.require("... and with ε_d = −0.5 the revenue rises to the cap", revenue_maximising(Market{1000, 100, -0.5}, 300).at_cap);
  }
  {
    // The design of levies (ADR 0162).
    const auto B = banded({{"cheap", Market{1000, 50, -1}, 2}, {"dear", Market{500, 200, -1}, 2}});
    s.check("a flat levy of 2 by band: 4% of a cheap night's price and 1% of a dear one's; the revenue the classes' sum",
            std::fabs(B.share_of_price[0] - 0.04) + std::fabs(B.share_of_price[1] - 0.01) +
                std::fabs(B.revenue - (2 * 1000 * 50.0 / 52 + 2 * 500 * 200.0 / 202)), 1e-9);
    const auto C = cap_on_nights(Market{1000, 100, -1}, 800);
    s.check("a cap of 800 nights, passed on in full at ε_d = −1: the levy that holds it 25, the scarcity rent 25 · 800",
            std::fabs(C.equivalent.levy - 25) + std::fabs(C.rent - 20000) / 1000 + std::fabs(C.equivalent.nights_after - 800), 1e-6);
    const auto D = dual_prices({{"residents", 1000, 10, -0.5, 10}, {"visitors", 500, 10, -2, 20}}, 5);
    s.check("dual prices: visitors charged twice the reference at ε = −2 come a quarter as often (125), lose 2,500 of surplus; residents none",
            std::fabs(D[1].visits - 125) + std::fabs(D[1].surplus_lost - 2500) + std::fabs(D[0].visits - 1000) + std::fabs(D[0].surplus_lost), 1e-9);
    s.require("... Lerner's rule at a cost of 5: 10 for the visitors (ε = −2), none for the residents (inelastic)",
              std::fabs(D[1].best_price - 10) < 1e-12 && std::isnan(D[0].best_price));
  }
  s.require("demand that rises with price is refused", refused([] { flat(Market{1000, 100, 0.5}, 1); }));
  s.require("a product not in the table is refused", refused([&] { demand(A, {{"z", {{"DOM", 1}}, 1}}, {{"DOM", 1}}); }));
  return s.done();
}
