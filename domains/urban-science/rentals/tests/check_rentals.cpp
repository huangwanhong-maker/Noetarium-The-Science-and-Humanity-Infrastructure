// Checks of urban::rentals, each worked by hand: a listing's nights from its reviews, the stay and its minimum, and the cap; homes
// withdrawn by nights and by host; hosts' concentration; areas summing to the city; a higher review rate never more nights; refusals.

#include <urban/rentals/rentals.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace urban::rentals;

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
  Reading r{"half", 0.5, 3, true, 0.7, 90, 0, ""};
  s.check("20 reviews at a rate of one half, stays of 3 nights: 120 nights", booked(Listing{"a", "entire", "h", 20, 1}, r) - 120, 1e-12);
  s.check("... a minimum of 5 nights makes each stay 5: 200", booked(Listing{"a", "entire", "h", 20, 5}, r) - 200, 1e-12);
  s.check("... 100 reviews would be 600 nights, capped at 70% of the year, 255.5", booked(Listing{"a", "entire", "h", 100, 1}, r) - 255.5, 1e-12);
  {
    const std::vector<Listing> ls = {{"a", "entire", "h1", 20, 1, 100, 3}, {"a", "entire", "h1", 10, 1, 80, 3}, {"b", "private", "h2", 40, 1, 50, 1},
                                     {"b", "entire", "h3", 0, 1, unknown, 1}};
    const auto R = assess(ls, r);
    s.check("nights 120, 60, 240 and 0; the entire home booked 120 > 90 withdrawn, the rest not: one",
            std::fabs(R.nights_total - 420) + std::fabs(R.withdrawn - 1) + std::fabs(R.entire - 3), 1e-12);
    Reading host = r;
    host.withdrawn_host = 3;
    s.check("... with hosts of three listings counted, the second home of h1 withdrawn too: two", std::fabs(assess(ls, host).withdrawn - 2), 1e-12);
    s.check("hosts' shares 1/2, 1/4, 1/4: HHI 0.375, the largest 0.5; h1, a multi-listing host, holds 1/2",
            std::fabs(R.hhi - 0.375) + std::fabs(R.largest_host - 0.5) + std::fabs(R.multi_host_share - 0.5), 1e-12);
    s.check("revenue over priced listings: 120 · 100 + 60 · 80 + 240 · 50 on 420 nights",
            std::fabs(R.revenue - (12000 + 4800 + 12000)) + std::fabs(R.priced_nights - 420), 1e-9);
    double nights = 0, listings = 0;
    for (const auto& [k, a] : R.areas) nights += a.nights, listings += a.listings;
    s.check("areas sum to the city", std::fabs(nights - R.nights_total) + std::fabs(listings - 4), 1e-12);
    Reading more = r;
    more.review_rate = 0.7;
    s.require("a higher review rate reads fewer nights", assess(ls, more).nights_total < R.nights_total);
  }
  s.require("a review rate of nought is refused", refused([] { assess({Listing{"a", "entire", "h", 1, 1}}, Reading{"x", 0, 3, true, 0.7, 90, 0, ""}); }));
  return s.done();
}
