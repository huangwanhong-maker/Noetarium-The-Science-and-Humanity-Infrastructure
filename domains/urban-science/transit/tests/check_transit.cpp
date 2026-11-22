// Checks of urban::transit, each worked by hand: a regular headway's mean wait half the headway; an irregular one's by Proposition 1;
// arrivals after the day's last departure unserved; a window with no departure in it; the proposition against a fine sum over arrivals
// on random timetables; SDG 11.2.1 along a street and in a straight line; a stop dropped by the test of frequency; a tram read as of
// low or of high capacity; refusals.

#include <urban/transit/transit.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace urban::transit;

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
std::vector<double> every(double from, double to, double step) {
  std::vector<double> t;
  for (double x = from; x <= to + 1e-9; x += step) t.push_back(x);
  return t;
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- service ----
  {
    const auto v = service(every(0, 7200, 600), 1800, 5400);
    s.check("every ten minutes, read from 0:30 to 1:30: six an hour, a mean wait of 300 s, the longest 600 s",
            std::fabs(v.per_hour - 6) + std::fabs(v.mean_wait - 300) + std::fabs(v.longest_wait - 600) + (v.continues ? 0 : 1), 1e-12);
    const auto w = service({2, 10}, 0, 10);
    s.check("departures at 2 and 10 read over [0, 10): (2² + 8²)/2/10 = 3.4", std::fabs(w.mean_wait - 3.4), 1e-12);
    const auto x = service({100, 200}, 0, 300);
    s.require("no departure after 200: arrivals after it unserved, the mean wait infinite and said", !x.continues && !std::isfinite(x.mean_wait));
    const auto y = service({500}, 0, 300);
    s.check("none within [0, 300), one at 500: the mean wait 500 − 150", std::fabs(y.mean_wait - 350) + y.departures, 1e-12);
    calx::random::Stream r(151, 1, 0, 0);
    double worst = 0;
    for (int trial = 0; trial < 50; trial++) {
      std::vector<double> t;
      for (int k = 0; k < 12; k++) t.push_back(4000 * r.uniform());
      t.push_back(5000);
      const double a = 500 + 500 * r.uniform(), b = a + 1500 + 1000 * r.uniform();
      const auto v2 = service(t, a, b);
      // The mean over 200,000 arrivals at the midpoints of equal steps.
      std::sort(t.begin(), t.end());
      const int n = 200000;
      double sum = 0;
      for (int k = 0; k < n; k++) {
        const double u = a + (b - a) * (k + 0.5) / n;
        sum += *std::lower_bound(t.begin(), t.end(), u) - u;
      }
      worst = std::max(worst, std::fabs(v2.mean_wait - sum / n) / (sum / n));
    }
    s.check("Proposition 1 against the mean over 200,000 arrivals, on 50 random timetables", worst, 1e-4);
  }

  // ---- SDG 11.2.1 ----
  const double peak_a = 7 * 3600, peak_b = 9 * 3600;
  const auto frequent = every(6 * 3600, 22 * 3600, 600);
  {
    // A street of 3 km along x: a bus stop at 0, a railway station at 2,000; six places of ten people.
    const auto walk = walking({{0, 0, 3000, 0}});
    const std::vector<Stop> stops = {{"bus", "bus", 0, 0, {{"bus", frequent}}}, {"rail", "rail", 2000, 0, {{"rail", frequent}}}};
    std::vector<Place> places;
    for (const double x : {0.0, 400.0, 800.0, 1200.0, 2000.0, 2900.0}) places.push_back({x, 0, 10});
    const auto R = access_share(walk, places, stops, metadata_reading(false), peak_a, peak_b);
    s.check("along a street: 0 and 400 m from the bus, 1,200, 2,000 and 2,900 within 1 km of the railway, 800 m from neither: 5/6",
            std::fabs(R.share - 5.0 / 6) + std::fabs(R.to_low[1] - 400) + std::fabs(R.to_high[3] - 800), 1e-9);
  }
  {
    // An L: a place 400 m up one street, a bus stop 300 m along the other, the corner at the origin.
    const auto walk = walking({{0, 0, 0, 1000}, {0, 0, 1000, 0}});
    const std::vector<Stop> stops = {{"b", "b", 300, 0, {{"bus", frequent}}}};
    const std::vector<Place> places = {{0, 400, 1}};
    const auto net = access_share(walk, places, stops, metadata_reading(false), peak_a, peak_b);
    const auto line = access_share(walk, places, stops, metadata_reading(false, never, false), peak_a, peak_b);
    s.check("round a corner: 700 m along the streets, 500 m in a straight line", std::fabs(net.to_low[0] - 700) + std::fabs(line.to_low[0] - 500), 1e-9);
    s.require("... so that the place is within 500 m only in a straight line", net.share == 0 && line.share == 1);
  }
  {
    const auto walk = walking({{0, 0, 2000, 0}});
    const std::vector<Stop> stops = {{"often", "often", 0, 0, {{"bus", every(6 * 3600, 22 * 3600, 1800)}}},
                                     {"seldom", "seldom", 1500, 0, {{"bus", every(6 * 3600, 22 * 3600, 3600)}}}};
    const std::vector<Place> places = {{1500, 0, 1}, {100, 0, 1}};
    const auto all = access_share(walk, places, stops, metadata_reading(false), peak_a, peak_b);
    const auto freq = access_share(walk, places, stops, metadata_reading(false, 20 * 60), peak_a, peak_b);
    s.require("a bus every hour waits 30 minutes on average: dropped by the test of 20, the bus every half hour (15) kept", all.low_stops == 2 && freq.low_stops == 1 && all.served[0] && !freq.served[0]);
  }
  {
    // A tram stop 900 m along a street from the one place: within 500 m it is not, within 1 km it is.
    const auto walk = walking({{0, 0, 2000, 0}});
    const std::vector<Stop> stops = {{"tram", "tram", 900, 0, {{"tram", frequent}}}};
    const std::vector<Place> places = {{1800, 0, 1}};
    const auto low = access_share(walk, places, stops, metadata_reading(false), peak_a, peak_b);
    const auto high = access_share(walk, places, stops, metadata_reading(true), peak_a, peak_b);
    s.require("a tram stop 900 m away serves the place only when a tram is read as of high capacity",
              !low.served[0] && std::fabs(low.to_low[0] - 900) < 1e-9 && high.served[0] && std::fabs(high.to_high[0] - 900) < 1e-9);
  }
  s.require("a window that ends before it begins is refused", refused([] { service({1, 2}, 5, 5); }));
  s.require("a way of one point is refused", refused([] { walking({{0, 0}}); }));
  return s.done();
}
