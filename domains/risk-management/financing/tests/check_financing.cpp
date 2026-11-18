// Checks of financing (financing.hpp).
//
// - A strategy by hand: losses 10, 50 and 200 in 100 years; insurance on [20, 100] at twice its expected loss with a fee of 0.5; what
//   is left unfunded costs 1 a unit up to 50 and 3 beyond: the expected cost a year is 2.7 + 2.9 = 5.6, the funded share 0.42.
// - The year of an event: what the instruments pay and what is unfunded sum to the event's loss.
// - Comparing strategies: each saving is the baseline's cost less the strategy's, and the cheapest is named.
// - The best tower of two instruments equals the least found by listing every choice of boundaries by hand, and a cap on the
//   100-year event's unfunded part is met.
// - Parametric cover: an index equal to the loss gives back the indemnity layer with no basis risk; a noisy index has some, and its
//   premium is the multiple times what it pays.
// - Refusals: layers that overlap, a rate on a limit that does not exist, tiers that end.

#include <risk_management/financing/financing.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rf = risk_management::financing;

namespace {
bool throws(auto&& f) {
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
  const double inf = std::numeric_limits<double>::infinity();
  const std::vector<rf::Tier> tiers{{50, 1}, {inf, 3}};
  {
    rf::Instrument ins{"cover", rf::Kind::insurance, 20, 80, 0.5, 0, 2, 0};
    const rf::Strategy st{"insured", {ins}, tiers};
    const auto C = rf::cost(st, {10, 50, 200}, 100, {50, 100});
    s.check("insurance by hand: 0.5 + 2 × 1.1 = 2.7 a year", C.instruments[0].expected - 2.7, 1e-14);
    s.check("unfunded: (10 + 20 + 50 + 3 × 70) / 100 = 2.9 a year", C.unfunded_cost - 2.9, 1e-14);
    s.check("the strategy costs 5.6 a year", C.expected - 5.6, 1e-14);
    s.check("and funds 1 − 1.5/2.6 of the expected loss", C.funded_share - (1 - 1.5 / 2.6), 1e-14);
    bool sums = true;
    for (std::size_t k = 0; k < C.return_periods.size(); k++) {
      double paid = 0;
      for (double p : C.paid_at[k]) paid += p;
      sums = sums && std::fabs(paid + C.unfunded_at[k] - C.loss_at[k]) < 1e-9;
    }
    s.require("in the year of an event, what is paid and what is unfunded sum to its loss", sums);
  }
  calx::random::Stream rng(5, 6, 7, 8);
  std::vector<double> losses;
  for (int i = 0; i < 400; i++) losses.push_back(rng.lognormal(13, 1.4));
  const double years = 2000;
  const rf::Instrument budget{"contingency budget", rf::Kind::budget, 0, inf, 0, 0.04, 0, 1};
  const rf::Instrument cover{"insurance", rf::Kind::insurance, 0, inf, 2e4, 0, 1.6, 0};
  const std::vector<rf::Tier> costly{{5e5, 1.2}, {inf, 2.5}};
  {
    std::vector<rf::Strategy> list;
    rf::Instrument b = budget;
    b.limit = 3e5;
    rf::Instrument c = cover;
    c.attachment = 3e5;
    c.limit = 3e6;
    list.push_back({"budget only", {b}, costly});
    list.push_back({"budget and insurance", {b, c}, costly});
    const auto K = rf::compare(list, losses, years, {10, 100});
    bool ok = true;
    for (std::size_t k = 0; k < list.size(); k++) ok = ok && std::fabs(K.saving[k] - (K.baseline.expected - K.strategies[k].expected)) < 1e-9;
    const std::size_t cheapest = K.strategies[0].expected <= K.strategies[1].expected ? 0 : 1;
    s.require("each saving is the baseline's cost less the strategy's, and the cheapest is named", ok && K.cheapest == cheapest);
  }
  {
    const std::vector<double> cand{1e5, 3e5, 1e6, 3e6};
    const auto best = rf::best_tower({budget, cover}, cand, costly, losses, years);
    // By hand: b1 ≤ b2 over {0, cand}, the budget on [0, b1), insurance on [b1, b2).
    std::vector<double> lv{0, 1e5, 3e5, 1e6, 3e6};
    double least = inf;
    for (std::size_t i = 0; i < lv.size(); i++)
      for (std::size_t j = i; j < lv.size(); j++) {
        rf::Strategy st{"", {}, costly};
        if (i > 0) {
          rf::Instrument b = budget;
          b.attachment = 0, b.limit = lv[i];
          st.instruments.push_back(b);
        }
        if (j > i) {
          rf::Instrument c = cover;
          c.attachment = lv[i], c.limit = lv[j] - lv[i];
          st.instruments.push_back(c);
        }
        if (st.instruments.empty()) continue;
        least = std::min(least, rf::cost(st, losses, years).expected);
      }
    s.check("the best tower of two equals the least found by listing every choice of boundaries", best.cost.expected - least, 1e-9);
    rf::TowerOptions o;
    o.cap_period = 100, o.cap = 0, o.top_unbounded = true;
    const auto capped = rf::best_tower({budget, cover}, cand, costly, losses, years, o);
    s.require("a cap on the 100-year event's unfunded part is met", capped.cost.unfunded_at.front() <= 1e-9 && capped.feasible <= capped.tried);
  }
  {
    std::vector<double> with(losses), noisy;
    with.push_back(2e5);
    with.push_back(1.2e6);  // the layer's ends among the events, so that the quantiles reach them
    const auto P = rf::parametric(with, with, years, 2e5, 1e6, 1.5, with.size());
    s.check("an index equal to the loss gives back the indemnity layer: no basis risk", P.risk.rmse / std::max(P.covered, 1.0), 1e-12);
    for (double v : losses) noisy.push_back(v * std::exp(rng.normal(0, 0.5)));
    const auto Q = rf::parametric(noisy, losses, years, 2e5, 1e6, 1.5, 64);
    s.require("a noisy index carries basis risk, and its premium is the multiple times what it pays",
              Q.risk.rmse > P.risk.rmse && std::fabs(Q.premium - 1.5 * Q.expected_paid) < 1e-9 && Q.risk.correlation < 1);
  }
  {
    rf::Instrument a{"a", rf::Kind::budget, 0, 100, 0, 0, 0, 1}, b{"b", rf::Kind::insurance, 50, 100, 0, 0, 1, 0}, c{"c", rf::Kind::reserve, 0, inf, 0, 0.1, 0, 1};
    s.require("refusals: layers that overlap, a rate on a limit that does not exist, tiers that end",
              throws([&] { rf::cost({"x", {a, b}, tiers}, {1}, 1); }) && throws([&] { rf::cost({"x", {c}, tiers}, {1}, 1); }) &&
                  throws([&] { rf::cost({"x", {a}, {{10, 1}}}, {1}, 1); }));
  }
  return s.done();
}
