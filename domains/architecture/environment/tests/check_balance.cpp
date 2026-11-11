// Checks of the day's heat balance of an envelope (balance.hpp), on a box known by hand.
//
// - A 10 × 10 box 9 m tall alone through a night of 24 h, 15 K colder outside: (1.2 · 360 + 0.6 · 100) · 15 · 24 Wh.
// - A neighbour against its east face, taller than it: that wall is a party wall and drops out; one 4.5 m tall covers its four lowest
//   rows of receivers, a threshold on the neighbour's height.
// - The Sun due east at 30°: the need falls by the sol-air gains on the east face and the roof, I cos 30° per m² of wall and I sin 30°
//   per m² of roof, through glass and opaque fabric.

#include <architecture/environment/balance.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>
#include <utility>
#include <vector>

namespace ae = architecture::environment;

namespace {
ae::Ring box(double x0, double y0, double x1, double y1) { return {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}; }
}  // namespace

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180;
  const auto R = ae::faces(box(0, 0, 10, 10), 9, 0, 1, 1);  // 360 receivers of 1 m²
  std::vector<std::pair<std::size_t, double>> walls;
  for (std::size_t r = 0; r < R.size(); r++) walls.push_back({r, 1.0});
  ae::Day night;
  night.night_air = 5, night.night_hours = 24, night.inside = 20;
  {
    const std::vector<ae::Prism> P{{box(0, 0, 10, 10), 9, -1}};
    const auto S = ae::shading(R, {}, P, 500);
    const auto C = ae::covering(R, P);
    s.check("a 10 m box 9 m tall alone, a night 15 K colder: (1.2 · 360 + 0.6 · 100) · 15 · 24 Wh", ae::heat_need(S, C, {}, walls, 100, night) - (1.2 * 360 + 0.6 * 100) * 15 * 24, 1e-6);
  }
  {
    const std::vector<ae::Prism> P{{box(0, 0, 10, 10), 9, -1}, {box(10, 0, 20, 10), 12, -1}};
    const auto S = ae::shading(R, {}, P, 500);
    const auto C = ae::covering(R, P);
    s.check("... a taller neighbour against its east face makes it a party wall: 270 m² of wall open", ae::heat_need(S, C, {}, walls, 100, night) - (1.2 * 270 + 0.6 * 100) * 15 * 24, 1e-6);
    const std::vector<ae::Prism> V{{box(0, 0, 10, 10), 9, -1}, {box(10, 0, 20, 10), 0, 0}};
    const auto SV = ae::shading(R, {}, V, 500);
    const auto CV = ae::covering(R, V);
    s.check("... a neighbour of variable height 4.5 m covers the wall's four lowest rows: 320 m² open", ae::heat_need(SV, CV, {4.5}, walls, 100, night) - (1.2 * 320 + 0.6 * 100) * 15 * 24, 1e-6);
  }
  {
    const std::vector<ae::Prism> P{{box(0, 0, 10, 10), 9, -1}};
    ae::Day cold;
    cold.air = {-30}, cold.inside = 20;
    const auto lit = ae::shading(R, {{30 * deg, 90 * deg, 1}}, P, 500), dark = ae::shading(R, {{-5 * deg, 90 * deg, 1}}, P, 500);
    const auto C = ae::covering(R, P);
    const architecture::space::Fabric F;
    const double I = ae::clear_sky(30 * deg);
    const double through = F.glazing * F.gain_coefficient + (1 - F.glazing) * F.u.at("facade") * F.absorptance / F.outer_coefficient;
    const double gains = 90 * I * std::cos(30 * deg) * through + 100 * I * std::sin(30 * deg) * F.u.at("roof") * F.absorptance / F.outer_coefficient;
    s.check("the Sun due east at 30° lowers the need by the sol-air gains on the east face and the roof",
            (ae::heat_need(dark, C, {}, walls, 100, cold) - ae::heat_need(lit, C, {}, walls, 100, cold)) - gains, 1e-6);
  }
  return s.done();
}
