// Checks of the rules of habitation (habitability.hpp), on the house of house.hpp, worked by hand.
//
// - A code that states nothing finds nothing.
// - Ceilings: 2.7 m meets 2.4 m; a kind given its own limit takes it.
// - Light: the living room's 5.04 m² of glass on 45.63 m² of floor is 11.0%, the bedroom's 1.8 on 30.03 is 6.0%: against 10%, one
//   passes and one fails. Air: 9.5% and 3.0% against 5%.
// - Doors: the entrance's 0.9 m and the inner door's 0.82 m against 0.82 m (met exactly).
// - A stair of 17 risers in 3 m: R = 176.5 mm, 2R + G = 603 mm with a going of 250, within 115–190 and 550–700; of 15 risers, R = 200.
// - A passage's width, and a kind's least area and width.

#include "house.hpp"

#include <architecture/plan/habitability.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>

namespace ap = architecture::plan;
using architecture::rules::Finding;
using architecture::rules::Limit;

namespace {
const Finding* find(const std::vector<Finding>& fs, const std::string& rule, const std::string& subject) {
  for (const auto& f : fs)
    if (f.rule == rule && f.subject.find(subject) != std::string::npos) return &f;
  return nullptr;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto b = house::building();
  const auto R = ap::read(b);

  s.require("a code that states nothing finds nothing", ap::check(b, R, ap::Code{}).empty());

  ap::Code c;
  c.ceiling_habitable = Limit::at_least(2.4, "10.3.1"), c.ceiling_other = Limit::at_least(2.1, "10.3.1");
  c.light = Limit::at_least(0.10, "10.5.1"), c.air = Limit::at_least(0.05, "10.6.2");
  c.entrance = Limit::at_least(0.82), c.door = Limit::at_least(0.82);
  c.area["bedroom"] = Limit::at_least(9), c.width["bedroom"] = Limit::at_least(3);
  const auto fs = ap::check(b, R, c);

  const auto* ceiling = find(fs, "ceiling", "Living");
  s.require("the living room's ceiling of 2.7 m meets 2.4 m, by its clause", ceiling && ceiling->ok && ceiling->actual == 2.7 && ceiling->clause == "10.3.1");
  const auto* ll = find(fs, "light", "Living");
  const auto* lb = find(fs, "light", "Bedroom");
  s.require("light is found of both rooms", ll && lb);
  s.check("the living room's glass is 5.04 / 45.63 of its floor", ll->actual - 5.04 / (5.85 * 7.8), 1e-12);
  s.check("the bedroom's is 1.8 / 30.03", lb->actual - 1.8 / (3.85 * 7.8), 1e-12);
  s.require("against a tenth, the living room passes and the bedroom fails", ll->ok && !lb->ok);
  const auto* al = find(fs, "air", "Living");
  const auto* ab = find(fs, "air", "Bedroom");
  s.require("against a twentieth of openable area, likewise", al && ab && al->ok && !ab->ok);
  s.check("... the bedroom's being 0.9 / 30.03", ab->actual - 0.9 / (3.85 * 7.8), 1e-12);
  const auto* entrance = find(fs, "entrance", "door 1");
  const auto* door = find(fs, "door", "door 2");
  s.require("the entrance of 0.9 m and the inner door of 0.82 m both meet 0.82 m", entrance && entrance->ok && door && door->ok && door->actual == 0.82);
  const auto* area = find(fs, "area", "Bedroom");
  const auto* width = find(fs, "width", "Bedroom");
  s.require("the bedroom meets its least area of 9 m² and its least width of 3 m", area && area->ok && width && width->ok);
  s.check("... its width found as 3.85 m", width->actual - 3.85, 1e-6, "m");
  s.require("no finding is made of a rule not stated: no passage, no stair", !find(fs, "passage", "") && !find(fs, "riser", ""));

  // A low ceiling fails the habitable rooms; a kind with its own limit takes it.
  {
    ap::Building low = b;
    low.storeys[0].ceiling = 2.3;
    ap::Code k = c;
    k.ceiling["bedroom"] = Limit::at_least(2.2);
    const auto f2 = ap::check(low, ap::read(low), k);
    s.require("at 2.3 m the living room fails 2.4 m, and the bedroom meets the 2.2 m stated for bedrooms", !find(f2, "ceiling", "Living")->ok && find(f2, "ceiling", "Bedroom")->ok);
  }

  // A passage: the living room read as a hall of 5.85 m; a stair.
  {
    ap::Building two = b;
    two.storeys[0].labels[0].kind = "hall";
    ap::Storey up;
    up.vertices = {{0, 0}, {6, 0}, {6, 8}, {0, 8}};
    for (std::size_t k = 0; k < 4; k++) up.walls.push_back(house::wall(k, (k + 1) % 4, 0.2));
    two.storeys.push_back(up);
    ap::Stair stair;
    stair.from = 0, stair.at = {1, 1}, stair.bearing = std::numbers::pi / 2, stair.going = 0.25, stair.risers = 17;
    two.stairs.push_back(stair);
    ap::Code k;
    k.passage = Limit::at_least(1.0);
    k.riser = Limit::between(0.115, 0.190), k.going = Limit::between(0.240, 0.355), k.slope = Limit::between(0.550, 0.700), k.flight = Limit::between(2, 18);
    const auto f3 = ap::check(two, ap::read(two), k);
    const auto* passage = find(f3, "passage", "Living");
    s.require("a hall is asked its clear width, and has it", passage && passage->ok);
    s.check("... 5.85 m", passage->actual - 5.85, 1e-6, "m");
    const auto* riser = find(f3, "riser", "stair 1");
    const auto* slope = find(f3, "slope", "stair 1");
    s.check("17 risers in 3 m are 176.5 mm each", riser->actual - 3.0 / 17, 1e-15, "m");
    s.check("... and 2R + G is 603 mm", slope->actual - (6.0 / 17 + 0.25), 1e-15, "m");
    s.require("... riser, going, slope and flight all within their ranges", riser->ok && slope->ok && find(f3, "going", "stair 1")->ok && find(f3, "flight", "stair 1")->ok);
    two.stairs[0].risers = 15;
    const auto f4 = ap::check(two, ap::read(two), k);
    s.require("15 risers of 200 mm fail the riser's range, and 2R + G of 650 mm still passes", !find(f4, "riser", "stair 1")->ok && find(f4, "slope", "stair 1")->ok);
  }
  return s.done();
}
