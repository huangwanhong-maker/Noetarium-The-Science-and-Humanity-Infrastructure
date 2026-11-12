// Checks of how high a building may rise before it takes the sun (solar.hpp), worked by hand.
//
// A street running east and west. North of it a building on [0, 100] × [10, 30], 20 m high; south of it one on [0, 60] × [−22, −8],
// 8 m high. Three points on the ground: N = (30, 7) and F = (80, 7) on the north footpath, S = (30, −7) on the south one. The Sun
// stands due south twice, an hour each: at 30° and at 25°.
//
// - From N a ray to the south enters the south building after 15 m: it is shaded when that building is higher than 15 tan α:
//   8.660 m at 30°, 6.995 m at 25°. At 8 m it has the hour at 30° alone. From S the building is 1 m away: no sun. From F the ray
//   passes east of the building: two hours.
// - Asked to keep one hour where there is one: the south building may rise to 15 tan 30° = 8.660 m, set by N; it reaches no other
//   point that has an hour (S has none to keep). The north building shades nothing of these: no limit.
// - Asked to keep two hours: N has not two, so nothing is asked of the south building: no limit.
// - Were the south building 6 m, N would have both hours; to keep two it may rise to 15 tan 25° = 6.995 m, to keep one to 8.660 m.
// - A tower on [20, 40] × [−60, −50] of 30 m, 57 m south of N: it shades N when higher than 57 tan α: 32.909 m at 30°, 26.580 m at
//   25°. At 30 m it takes the hour at 25°, whatever the south building does; N has the hour at 30°. To keep that hour the south
//   building may rise to 8.660 m and the tower to 32.909 m. At 60 m the tower takes both hours, and nothing is asked of either.

#include <architecture/environment/solar.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>

namespace ae = architecture::environment;

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180, t30 = std::tan(30 * deg), t25 = std::tan(25 * deg);
  const std::vector<ae::Receiver> points = {{30, 7, 0, 0, 0, -1}, {30, -7, 0, 0, 0, -1}, {80, 7, 0, 0, 0, -1}};
  const std::vector<ae::Sun> suns = {{30 * deg, 180 * deg, 1.0}, {25 * deg, 180 * deg, 1.0}};
  std::vector<ae::Prism> prisms = {{{{0, 10}, {100, 10}, {100, 30}, {0, 30}}, 20, 0}, {{{0, -22}, {60, -22}, {60, -8}, {0, -8}}, 8, 1}};
  const ae::Shading S = ae::shading(points, suns, prisms, 400);
  {
    const auto hours = ae::sun_hours(S, {20, 8});
    s.check("the north footpath has the hour at 30°, the south one none, and the point beyond the building both", std::fabs(hours[0] - 1) + hours[1] + std::fabs(hours[2] - 2), 1e-12, "h");
    const auto one = ae::solar_limits(S, {20, 8}, 1.0);
    s.check("to keep one hour the south building may rise to 15 tan 30°", one[1].height - 15 * t30, 1e-5, "m");
    s.require("... set by the point north of it, the only one with an hour that it reaches", one[1].receiver == 0 && one[1].reached == 1);
    s.require("... and the north building, which shades none of them, has no limit", std::isinf(one[0].height) && one[0].reached == 0);
    s.check("... as it stands it takes two hours from the point at its foot and one from the point across: three in all", one[1].taken - 3, 1e-12, "h");
    s.check("... and the north building takes none", one[0].taken, 1e-12, "h");
    const auto two = ae::solar_limits(S, {20, 8}, 2.0);
    s.require("to keep two hours nothing is asked of it: the point has not two to keep", std::isinf(two[1].height) && two[1].reached == 0);
  }
  {
    s.check("were it 6 m, to keep two hours it may rise to 15 tan 25°", ae::solar_limits(S, {20, 6}, 2.0)[1].height - 15 * t25, 1e-5, "m");
    s.check("... and to keep one, to 15 tan 30°", ae::solar_limits(S, {20, 6}, 1.0)[1].height - 15 * t30, 1e-5, "m");
    s.require("a limit is never below the height that stands", ae::solar_limits(S, {20, 6}, 2.0)[1].height >= 6 && ae::solar_limits(S, {20, 8}, 1.0)[1].height >= 8);
  }
  {
    prisms.push_back({{{20, -60}, {40, -60}, {40, -50}, {20, -50}}, 30, 2});
    const ae::Shading T = ae::shading(points, suns, prisms, 400);
    s.check("a tower of 30 m behind takes the hour at 25°: the point keeps the hour at 30°", ae::sun_hours(T, {20, 8, 30})[0] - 1, 1e-12, "h");
    const auto limits = ae::solar_limits(T, {20, 8, 30}, 1.0);
    s.check("... to keep it the south building may rise to 15 tan 30°", limits[1].height - 15 * t30, 1e-5, "m");
    s.check("... and the tower to 57 tan 30°", limits[2].height - 57 * t30, 1e-4, "m");
    const auto dark = ae::solar_limits(T, {20, 8, 60}, 1.0);
    s.require("at 60 m the tower takes both hours, and nothing is asked of either", std::isinf(dark[1].height) && std::isinf(dark[2].height));
  }
  return s.done();
}
