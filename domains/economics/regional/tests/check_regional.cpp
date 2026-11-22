// Checks of economics::regional, each worked by hand: a region holding 30% and 10% of two industries and 20% of all employment; its
// coefficients by the simple quotient, its purchases from the rest of the country, and every column summing as the country's; Flegg's
// λ* and FLQ; the augmented quotient above FLQ where the buyer is specialised; regional multipliers never above the national; households'
// local shares; the accounts' identity for a region, its purchases from the rest of the country a fourth leak; refusals.

#include <economics/regional/regional.hpp>
#include <economics/tourism/tourism.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace economics::regional;

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
}  // namespace

int main() {
  calx::verification::Suite s;
  economics::InputOutputTable T;
  T.sectors = {"a", "b"};
  T.z = calx::linalg::Matrix(2);
  T.z(0, 0) = 10, T.z(0, 1) = 20, T.z(1, 0) = 30, T.z(1, 1) = 0;
  T.final_demand = {170, 70};  // outputs 200 and 100
  T.value_added = {160, 80};
  const Region R{{0.3, 0.1}, 0.2};
  {
    const auto S = regionalise(T, R, Quotient::slq);
    s.check("SLQ 1.5 and 0.5; outputs 60 and 10", std::fabs(S.slq[0] - 1.5) + std::fabs(S.slq[1] - 0.5) + std::fabs(S.output[0] - 60) + std::fabs(S.output[1] - 10), 1e-12);
    s.check("by SLQ: flows 3, 2, 4.5, 0 within the region", std::fabs(S.local.z(0, 0) - 3) + std::fabs(S.local.z(0, 1) - 2) + std::fabs(S.local.z(1, 0) - 4.5) + S.local.z(1, 1), 1e-12);
    s.check("... the first column buys 4.5 from the rest of the country, the second nothing", std::fabs(S.rest[0] - 4.5) + S.rest[1], 1e-12);
    s.check("... each column's flows within and from the rest of the country sum to the national coefficients times its output",
            std::fabs(S.local.z(0, 0) + S.local.z(1, 0) + S.rest[0] - 0.2 * 60) + std::fabs(S.local.z(0, 1) + S.local.z(1, 1) + S.rest[1] - 0.2 * 10), 1e-12);
  }
  {
    const auto F = regionalise(T, R, Quotient::flq, 0.3);
    const double lambda = std::pow(std::log2(1.2), 0.3);
    s.check("FLQ: λ* = log2(1.2)^0.3; FLQ_ba = (0.5/1.5) λ*, FLQ_bb = 0.5 λ*, the first row capped at one",
            std::fabs(F.lambda - lambda) + std::fabs(F.quotient(1, 0) - lambda / 3) + std::fabs(F.quotient(1, 1) - 0.5 * lambda) + std::fabs(F.quotient(0, 0) - 1) + std::fabs(F.quotient(0, 1) - 1), 1e-12);
    const auto A = regionalise(T, R, Quotient::aflq, 0.3);
    s.check("AFLQ: where the buyer is specialised (SLQ_a = 1.5) FLQ times log2(2.5)", std::fabs(A.quotient(1, 0) - lambda / 3 * std::log2(2.5)) + std::fabs(A.quotient(1, 1) - F.quotient(1, 1)), 1e-12);
    const auto Mn = economics::leontief(T), Mr = economics::leontief(F.local);
    s.require("the region's output multipliers lie below the country's", Mr.multipliers[0] < Mn.multipliers[0] && Mr.multipliers[1] < Mn.multipliers[1]);
  }
  {
    const auto sh = local_share({0.8, 1.0}, {1.5, 0.5});
    s.check("households' local shares: 0.8 · min(1, 1.5) and 1 · min(1, 0.5)", std::fabs(sh[0] - 0.8) + std::fabs(sh[1] - 0.5), 1e-15);
  }
  {
    // The accounts' identity for the region: value added, imported inputs, taxes and purchases from the rest of the country sum to the
    // region's domestic demand, on the FLQ table with national coefficients of value added (0.8 and 0.8), taxes nought and imports nought.
    const auto F = regionalise(T, R, Quotient::flq, 0.3);
    economics::tourism::Accounts A;
    A.domestic = F.local;
    A.gva = {0.8 * F.output[0], 0.8 * F.output[1]}, A.taxes = {0, 0}, A.imported = {0, 0}, A.rest = F.rest;
    A.domestic_share = local_share({1, 1}, F.slq);
    A.rest_share = {1 - A.domestic_share[0], 1 - A.domestic_share[1]};
    const auto E = economics::tourism::effects(A, {0, 10});
    s.check("the region's accounts: value added, imported inputs, taxes and purchases from the rest of the country sum to its domestic demand",
            std::fabs(E.total_gva + E.induced_imports + E.induced_taxes + E.induced_rest - E.demand_domestic) + std::fabs(A.imbalance()), 1e-10);
    s.check("... 10 of b, half supplied in the region (SLQ 0.5), half from the rest of the country, nothing from abroad",
            std::fabs(E.demand_domestic - 5) + std::fabs(E.demand_rest - 5) + std::fabs(E.demand_imported), 1e-12);
  }
  s.require("a region of the whole country is refused", refused([&] { regionalise(T, Region{{0.3, 0.1}, 1.0}); }));
  s.require("a share for each product is asked", refused([&] { regionalise(T, Region{{0.3}, 0.2}); }));
  return s.done();
}
