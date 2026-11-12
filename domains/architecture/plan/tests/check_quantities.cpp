// Checks of a drawn building's quantities, cost and heat loss (quantities.hpp), on the house of house.hpp, worked by hand.
//
// - One storey of 10 × 8 × 3: 36 m of outer wall, 108 m² gross; its openings 1.836 (the entrance), 1.8 and 5.04 (the windows); the
//   dividing wall 8 × 2.7 less its door; the sides the walls and the glass face.
// - With a storey of 6 × 8 above: the roof is still 80 m² in plan (32 uncovered below, 48 above), and 80 / cos 30° pitched.
// - Cost is an interval, the sum of each quantity times its rate's ends; an element with no rate is left out.
// - H = Σ U A.

#include "house.hpp"

#include <architecture/plan/quantities.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <map>

namespace ap = architecture::plan;

int main() {
  calx::verification::Suite s;
  const auto b = house::building();
  const auto R = ap::read(b);
  const auto q = ap::quantities(b, R);
  const double entrance = 0.9 * 2.04, door = 0.82 * 2.04;

  s.check("the floor is 80 m², all of it the ground slab", std::fabs(q.floor_area - 80) + std::fabs(q.ground_slab - 80) + std::fabs(q.upper_floor), 1e-12, "m²");
  s.check("the outer walls run 36 m and stand 108 m² gross", std::fabs(q.outer_length - 36) + std::fabs(q.outer_wall - 108), 1e-12);
  s.check("... and net of the entrance and two windows", q.outer_wall_net - (108 - entrance - 1.8 - 5.04), 1e-12, "m²");
  s.check("the dividing wall is 8 × 2.7 less its door", q.inner_wall_net - (21.6 - door), 1e-12, "m²");
  s.require("two windows, one door to the outside, one between rooms, no stair", q.windows == 2 && q.outer_doors == 1 && q.inner_doors == 1 && q.stairs == 0);
  s.check("the windows are 6.84 m², all glass", std::fabs(q.window_area - 6.84) + std::fabs(q.glass_area - 6.84), 1e-12, "m²");
  s.check("the walls face north and south with 30 m² each, east and west with 24",
          std::fabs(q.wall_by_side[0] - 30) + std::fabs(q.wall_by_side[2] - 30) + std::fabs(q.wall_by_side[1] - 24) + std::fabs(q.wall_by_side[3] - 24), 1e-12, "m²");
  s.check("the bedroom's glass faces east, the living room's west",
          std::fabs(q.glass_by_side[1] - 1.8) + std::fabs(q.glass_by_side[3] - 5.04) + std::fabs(q.glass_by_side[0]) + std::fabs(q.glass_by_side[2]), 1e-12, "m²");
  s.require("the sides are the quadrants of the outward normal's bearing", ap::side_of(0, 1) == 0 && ap::side_of(1, 0) == 1 && ap::side_of(0, -1) == 2 && ap::side_of(-1, 0) == 3 && ap::side_of(1, 1.01) == 0);
  s.check("the roof covers the 80 m²", q.roof - 80, 1e-12, "m²");
  s.check("no room is wet", q.wet_area, 1e-12, "m²");

  // A storey above the living room.
  {
    ap::Building two = b;
    ap::Storey up;
    up.vertices = {{0, 0}, {6, 0}, {6, 8}, {0, 8}};
    for (std::size_t k = 0; k < 4; k++) up.walls.push_back(house::wall(k, (k + 1) % 4, 0.2));
    up.labels = {{{3, 4}, "bathroom", "Bath"}};
    two.storeys.push_back(up);
    const auto R2 = ap::read(two);
    const double pitch = 1 / std::cos(30 * std::numbers::pi / 180);
    const auto q2 = ap::quantities(two, R2, pitch);
    s.check("with 48 m² above, the floors are 128 m² and the upper floor 48", std::fabs(q2.floor_area - 128) + std::fabs(q2.upper_floor - 48), 1e-12, "m²");
    s.check("the roof in plan is 32 uncovered below and 48 above", q2.roof_plan - 80, 1e-12, "m²");
    s.check("... and pitched at 30°, 80 / cos 30°", q2.roof - 80 * pitch, 1e-12, "m²");
    s.check("the bathroom above is wet: 5.8 × 7.8 inside its walls", q2.wet_area - 5.8 * 7.8, 1e-12, "m²");
  }

  // Cost.
  {
    const std::map<std::string, ap::Rate> rates{{"outer_wall", {100, 200}}, {"window", {500, 1000}}, {"outer_door", {800, 2000}}};
    const auto c = ap::cost(q, rates);
    const double wall = 108 - entrance - 1.8 - 5.04;
    s.require("an element with no rate is left out: three items", c.items.size() == 3);
    s.check("the low cost is the sum of each quantity at its low rate", c.low - (100 * wall + 500 * 6.84 + 800), 1e-9);
    s.check("... and the high cost at its high rate", c.high - (200 * wall + 1000 * 6.84 + 2000), 1e-9);
    s.require("eleven elements can carry a rate", ap::elements(q).size() == 11);
  }

  // Heat.
  {
    ap::Fabric f;
    f.wall = 0.5, f.window = 2.8, f.door = 2.5, f.roof = 0.25, f.ground = 0.6;
    const auto h = ap::heat_loss(q, f);
    const double expect = 0.5 * (108 - entrance - 1.8 - 5.04) + 2.8 * 6.84 + 2.5 * entrance + 0.25 * 80 + 0.6 * 80;
    s.check("H = Σ U A over walls, windows, the door, the roof and the ground", h.total - expect, 1e-12, "W/K");
    s.check("... and per m² of floor", h.per_floor - expect / 80, 1e-12, "W/(m² K)");
  }
  return s.done();
}
