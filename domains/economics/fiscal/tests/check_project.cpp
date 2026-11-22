// Checks of economics::fiscal's project plans, each worked by hand: a cost met by receipts and a loan, its coverage year by year; an
// annuity's payment with interest; receipts that arrive after the loan, pledged to its service or kept in the reserve; a reserve that
// earns and pays a later cost; the largest scale with and without receipts in closed form; Proposition 1 on random plans; the floor
// failing without the project; a floor that never binds; refusals.

#include <economics/fiscal/project.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace economics::fiscal;

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
// Five years from 2026, a surplus of 100 and an existing service of 20 each year.
Projection flat(double surplus = 100, double existing = 20, int years = 5) {
  Projection p;
  for (int k = 0; k < years; k++) {
    p.years.push_back(2026 + k);
    p.totals.push_back({});
    p.surplus.push_back(surplus);
    p.debt_service.push_back(existing);
    p.coverage.push_back(existing > 0 ? surplus / existing : none);
    p.outstanding.push_back(0);
  }
  return p;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto P = flat();
  {
    Plan pl;
    pl.cost = {{2026, 300, "the line"}};
    pl.receipts = {{2026, 100, "levy"}};
    pl.rate = 0;
    pl.tenor = 4;
    const auto R = plan(P, pl);
    s.check("a cost of 300 met by 100 of receipts and a loan of 200, repaid 50 a year from 2027", std::fabs(R.borrowed - 200) + std::fabs(R.years[1].new_service - 50), 1e-12);
    s.check("... the coverage 100/20 = 5 in 2026 and 100/(20 + 50) after; the least in 2027",
            std::fabs(R.years[0].coverage - 5) + std::fabs(R.least_coverage - 100.0 / 70) + std::fabs(R.least_year - 2027), 1e-12);
    s.check("... the new loan repaid by 2030", R.new_debt_left, 1e-9);
    Plan interest = pl;
    interest.rate = 0.05;
    interest.tenor = 2;
    interest.receipts = {{2026, 200, "levy"}};
    const auto I = plan(P, interest);
    const double payment = 100 * 0.05 / (1 - std::pow(1.05, -2));
    s.check("an annuity of 100 at 5% over two years: 53.78 a year, coverage 100/(20 + 53.78)", std::fabs(I.years[1].new_service - payment) + std::fabs(I.years[1].coverage - 100 / (20 + payment)), 1e-9);
  }
  {
    Plan pl;
    pl.cost = {{2026, 300, "the line"}};
    pl.receipts = {{2027, 60, "tax increment"}};
    pl.rate = 0;
    pl.tenor = 4;
    pl.pledge = true;
    const auto R = plan(P, pl);
    s.check("receipts of 60 in 2027 pledged to the new service of 75: the budget carries 20 + 75 − 60, coverage 100/35",
            std::fabs(R.years[1].pledged - 60) + std::fabs(R.years[1].coverage - 100.0 / 35), 1e-12);
    pl.pledge = false;
    const auto K = plan(P, pl);
    s.check("... not pledged, they wait in the reserve, and the coverage is 100/95", std::fabs(K.years[1].reserve - 60) + std::fabs(K.years[1].coverage - 100.0 / 95) + std::fabs(K.reserve_left - 60), 1e-12);
  }
  {
    Plan pl;
    pl.cost = {{2026, 100, "first works"}, {2027, 150, "second works"}};
    pl.receipts = {{2026, 200, "sale of reserve land"}};
    pl.reserve_rate = 0.1;
    pl.rate = 0;
    pl.tenor = 4;
    const auto R = plan(P, pl);
    s.check("a reserve of 100 earning 10% pays 110 of the next year's 150; 40 borrowed", std::fabs(R.years[0].reserve - 100) + std::fabs(R.years[1].from_reserve - 110) + std::fabs(R.borrowed - 40), 1e-12);
  }
  {
    // Coverage at 2027: 100/(20 + (300 s − 100)/4) ≥ 2 iff s ≤ 220/300; without the receipts, s ≤ 120/300.
    Plan pl;
    pl.cost = {{2026, 300, "the line"}};
    pl.receipts = {{2026, 100, "levy"}};
    pl.rate = 0;
    pl.tenor = 4;
    const auto with = largest_scale(P, pl, 2.0);
    Plan none_ = pl;
    none_.receipts.clear();
    const auto without = largest_scale(P, none_, 2.0);
    s.check("the largest scale at a floor of 2: 220/300 with the receipts, 120/300 without", std::fabs(with.scale - 220.0 / 300) + std::fabs(without.scale - 120.0 / 300), 2e-6);
    s.require("... both bounded by the coverage of 2027", with.bounded && without.bounded && with.binding.find("2027") != std::string::npos);
    const auto weak = largest_scale(flat(30, 20), pl, 2.0);
    s.require("a coverage below the floor without the project: none affordable, and said", weak.fails_at_none && weak.scale == 0);
    Plan nothing;
    nothing.cost = {{2026, 0, "nothing"}};
    s.require("a project that costs nothing never binds: the floor holds at the largest scale tried", !largest_scale(P, nothing, 2.0).bounded);
  }
  {
    // Proposition 1 on random plans: the least coverage does not rise and the borrowing does not fall as the scale rises.
    calx::random::Stream r(153, 1, 0, 0);
    bool monotone = true;
    for (int trial = 0; trial < 200; trial++) {
      const auto Q = flat(60 + 80 * r.uniform(), 10 * r.uniform(), 8);
      Plan pl;
      pl.rate = 0.08 * r.uniform();
      pl.tenor = 1 + static_cast<int>(10 * r.uniform());
      pl.reserve_rate = 0.05 * r.uniform();
      pl.pledge = r.uniform() < 0.5;
      for (int y = 2026; y < 2034; y++) {
        if (r.uniform() < 0.5) pl.cost.push_back({y, 200 * r.uniform(), "works"});
        if (r.uniform() < 0.4) pl.receipts.push_back({y, 120 * r.uniform(), "receipts"});
        if (r.uniform() < 0.2) pl.grants.push_back({y, 80 * r.uniform(), "grant"});
      }
      double last_cov = std::numeric_limits<double>::infinity(), last_borrowed = -1;
      for (double sc = 0; sc <= 3.0001; sc += 0.25) {
        const auto R = plan(Q, pl, sc);
        const double cov = std::isfinite(R.least_coverage) ? R.least_coverage : std::numeric_limits<double>::infinity();
        monotone = monotone && cov <= last_cov + 1e-9 && R.borrowed >= last_borrowed - 1e-9;
        last_cov = cov, last_borrowed = R.borrowed;
      }
    }
    s.require("Proposition 1 on 200 random plans: the least coverage never rises, the borrowing never falls, as the scale rises", monotone);
  }
  {
    Plan bad;
    bad.cost = {{2040, 10, "too late"}};
    s.require("a cost outside the years ahead is refused", refused([&] { plan(P, bad); }));
    Plan neg;
    neg.receipts = {{2026, -5, "a refund"}};
    s.require("a negative receipt is refused", refused([&] { plan(P, neg); }));
  }
  return s.done();
}
