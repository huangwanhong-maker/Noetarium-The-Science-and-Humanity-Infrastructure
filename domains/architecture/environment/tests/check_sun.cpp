// Checks of sun and shadow (sun.hpp).
//
// - A box of 20 m in the sun at altitude α: a point on the ground behind it, along the Sun's direction, is shaded exactly while its
//   distance from the box is below 20 / tan α.
// - A face turned from the Sun is shaded; one facing it in the open is lit; a point on a face at height z, a building of height H at
//   distance d before it, is shaded exactly when z + d tan α < H.
// - Thresholds: for random heights of the buildings, the comparisons give what a direct test of each ray gives.
// - Hours: a receiver's hours are the weights of the instants it is lit.

#include <architecture/environment/sun.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>
#include <string>
#include <vector>

namespace ae = architecture::environment;

namespace {
ae::Ring box(double x0, double y0, double x1, double y1) { return {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}; }
}  // namespace

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180;
  {
    // A box 10 m square from x = 0 to 10, 20 m tall; the Sun due east (azimuth 90°) at 30°: shadows fall west.
    const std::vector<ae::Prism> P{{box(0, 0, 10, 10), 20, -1}};
    const double len = 20 / std::tan(30 * deg);
    std::vector<ae::Receiver> R;
    for (double d : {len - 0.5, len + 0.5}) R.push_back({-d, 5, 0, 0, 0, -1});
    const auto S = ae::shading(R, {{30 * deg, 90 * deg, 1}}, P, 500);
    s.require("a box of 20 m with the Sun at 30°: shaded just within its shadow's length 20/tan 30° (34.6 m), lit just beyond",
              !ae::sunlit(S, 0, 0, {}) && ae::sunlit(S, 1, 0, {}));
  }
  {
    // Faces: the east face of a box lit by an eastern Sun, its west face not; a point on a west-facing face 8 m up, a 20 m building 10 m
    // to its west, the Sun due west at α.
    const std::vector<ae::Prism> P{{box(0, 0, 10, 10), 12, -1}, {box(-20, 0, -10, 10), 20, -1}};
    std::vector<ae::Receiver> R{{10.05, 5, 6, 1, 0, 0}, {-0.05, 5, 6, -1, 0, 0}};
    const auto east = ae::shading(R, {{30 * deg, 90 * deg, 1}}, P, 500);
    s.require("a face turned from the Sun is shaded, one facing it in the open is lit", ae::sunlit(east, 0, 0, {}) && !ae::sunlit(east, 1, 0, {}));
    // West face at z = 8, the building's east face 10 m away: shaded when 8 + 10 tan α < 20, i.e. α < atan(1.2) = 50.19°.
    std::vector<ae::Receiver> W{{-0.05, 5, 8, -1, 0, 0}};
    const double edge = std::atan(1.2);
    const auto lo = ae::shading(W, {{edge - 0.01, 270 * deg, 1}}, P, 500), hi = ae::shading(W, {{edge + 0.01, 270 * deg, 1}}, P, 500);
    s.require("a point 8 m up a face, a 20 m building 10 m before it: shaded below atan 1.2, lit above", !ae::sunlit(lo, 0, 0, {}) && ae::sunlit(hi, 0, 0, {}));
  }
  {
    // Thresholds against a direct test: nine buildings of variable height on a grid, receivers on the ground, three instants.
    calx::random::Stream st(20261109, 0x73756e73u, 0, 0);  // "suns"
    std::vector<ae::Prism> P;
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++) P.push_back({box(30.0 * i, 30.0 * j, 30.0 * i + 15, 30.0 * j + 15), 0, 3 * i + j});
    const auto R = ae::ground(box(-20, -20, 100, 100), 4, P);
    const std::vector<ae::Sun> suns{{20 * deg, 30 * deg, 1}, {35 * deg, 0, 2}, {15 * deg, 300 * deg, 1}};
    const auto S = ae::shading(R, suns, P, 1000);
    std::size_t disagree = 0, cases = 0;
    for (int trial = 0; trial < 5; trial++) {
      std::vector<double> h(9);
      for (auto& v : h) v = st.uniform(3, 60);
      std::vector<ae::Prism> fixed = P;
      for (std::size_t c = 0; c < fixed.size(); c++) fixed[c].height = h[c], fixed[c].variable = -1;
      const auto D = ae::shading(R, suns, fixed, 1000);
      for (std::size_t r = 0; r < R.size(); r++)
        for (std::size_t k = 0; k < suns.size(); k++, cases++) disagree += ae::sunlit(S, r, k, h) != ae::sunlit(D, r, k, {});
    }
    s.require("thresholds for variable heights agree with a direct test of every ray: " + std::to_string(disagree) + " of " + std::to_string(cases), disagree == 0);
    const auto hours = ae::sun_hours(S, std::vector<double>(9, 0.0));
    bool all = true;
    for (double v : hours) all = all && v == 4;
    s.require("... and with every building at height zero every receiver has all 4 hours", all);
  }
  return s.done();
}
