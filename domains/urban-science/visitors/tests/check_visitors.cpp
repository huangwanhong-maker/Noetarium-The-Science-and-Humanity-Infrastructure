// Checks of urban::visitors, each worked by hand: the days of a month in leap and common years; a flat year's stock, its peak over the
// mean one and its Gini nought; a year whose visitors all come in July, its peak over the mean 365/31 and its Gini 334/365; the readings
// (unregistered stays, platform rentals, same-day visitors) each adding what it declares; a withheld month; intensity judged against a
// band; a region's nights allocated to places that partition it, summing to the region; the foreign share; refusals.

#include <urban/visitors/visitors.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace urban::visitors;

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
std::array<double, 12> each(double per_day, int year = 2023) {
  std::array<double, 12> a{};
  const auto d = days_in(year);
  for (int m = 0; m < 12; m++) a[m] = per_day * d[m];
  return a;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  s.require("February has 29 days in 2024 and 2000, 28 in 2023 and 1900", days_in(2024)[1] == 29 && days_in(2000)[1] == 29 && days_in(2023)[1] == 28 && days_in(1900)[1] == 28);
  {
    Nights n{{"registered", {{"DOM", each(600)}, {"FOR", each(400)}}}};
    Reading r{"registered", {"registered"}};
    const auto R = stock(n, 2023, r, 100000, 50, {{"per_thousand_mean", 0, 5, "a declared limit"}, {"gini", 0, 0.3, "a declared limit"}});
    s.check("a flat year of 1,000 nights a day: 1,000 present every month, the peak the mean, the Gini nought",
            std::fabs(R.mean - 1000) + std::fabs(R.peak - 1000) + std::fabs(R.peak_to_mean - 1) + std::fabs(R.gini), 1e-9);
    s.check("... 10 per thousand of 100,000 residents, 20 per km² of 50, 3.65 nights a resident; 40% foreign",
            std::fabs(R.per_thousand_mean - 10) + std::fabs(R.per_km2_mean - 20) + std::fabs(R.nights_per_resident - 3.65) + std::fabs(R.foreign_share - 0.4), 1e-9);
    s.require("... 10 per thousand above a band of 0 to 5, the Gini within 0 to 0.3", R.verdicts.at("per_thousand_mean") == "above" && R.verdicts.at("gini") == "within");
    s.check("... by origin: 600 domestic and 400 foreign present in March", std::fabs(R.months[2].by_origin.at("DOM") - 600) + std::fabs(R.months[2].by_origin.at("FOR") - 400), 1e-9);
  }
  {
    std::array<double, 12> july{};
    july[6] = 120 * 31;
    Nights n{{"registered", {{"DOM", july}}}};
    const auto R = stock(n, 2023, Reading{"july", {"registered"}});
    s.check("all visitors in July, 120 a day: the mean 120 · 31/365, the peak over the mean 365/31, the Gini 334/365",
            std::fabs(R.mean - 120.0 * 31 / 365) + std::fabs(R.peak_to_mean - 365.0 / 31) + std::fabs(R.gini - 334.0 / 365) + (R.peak_month == 7 ? 0 : 1), 1e-9);
  }
  {
    Nights n{{"registered", {{"DOM", each(1000)}}}, {"platform", {{"DOM", each(250)}}}};
    Reading reg{"registered", {"registered"}}, wide{"wide", {"registered", "platform"}, 0.2, 0.5, 6};
    const auto A = stock(n, 2023, reg), B = stock(n, 2023, wide);
    s.check("unregistered stays at 0.2 a registered night, platform rentals and same-day visitors at 0.5 a night for 6 hours: 1,200 + 250 overnight, 1,450 · 0.5 · 6/24 same-day",
            std::fabs(B.months[0].overnight - 1450) + std::fabs(B.months[0].same_day - 1450 * 0.5 * 0.25) + std::fabs(A.mean - 1000), 1e-9);
    s.check("... the platform's share of counted nights 250/1,250", std::fabs(B.platform_share - 0.2), 1e-12);
  }
  {
    auto v = each(100);
    v[3] = unknown;
    Nights n{{"registered", {{"DOM", v}}}};
    const auto R = stock(n, 2023, Reading{"withheld", {"registered"}});
    s.require("a month withheld: the month and the year said incomplete, the other months as given", !R.complete && !R.months[3].complete && R.months[2].complete && std::fabs(R.months[2].overnight - 100) < 1e-12);
  }
  {
    std::map<std::string, std::array<double, 12>> region{{"DOM", each(10)}}, k1{{"DOM", each(3)}}, k2{{"DOM", each(9)}}, kr{{"DOM", each(12)}};
    const auto a = allocate(region, k1, kr), b = allocate(region, k2, kr);
    s.check("a region's 310 nights in January carried by keys of 3 and 9 of 12: 77.5 and 232.5, summing to the region",
            std::fabs(a.at("DOM")[0] - 77.5) + std::fabs(b.at("DOM")[0] - 232.5) + std::fabs(a.at("DOM")[5] + b.at("DOM")[5] - region.at("DOM")[5]), 1e-9);
    s.require("a place's key above its region's is refused", refused([&] { allocate(region, kr, k1); }));
  }
  s.require("a reading of no source is refused", refused([] { stock(Nights{{"registered", {{"DOM", each(1)}}}}, 2023, Reading{"none", {}}); }));
  s.require("a band on a measure that is not there is refused", refused([] { stock(Nights{{"registered", {{"DOM", each(1)}}}}, 2023, Reading{"r", {"registered"}}, 1, 1, {{"beauty", 0, 1, ""}}); }));
  s.check("the mean stay: 3,000 nights over 1,000 arrivals", Stay{3000, 1000}.mean_stay() - 3, 1e-12);
  return s.done();
}
