// Checks of a drawn building's shell (shell.hpp), on the house of the plan's checks and an L, worked by hand.
//
// THE HOUSE (plan/tests/house.hpp): 10 × 8 m to the centrelines of outer walls 0.2 m thick, one storey of 3.0 m. Its outer faces
// stand on [−0.1, 10.1] × [−0.1, 8.1]: a footprint of 10.2 × 8.2 = 83.64 m² with four edges (the two walls of the south side, and
// of the north, are each one edge).
//
// - FLAT, a parapet of 0.3 m: the roof is the footprint, 83.64 m²; the walls rise 0.3 m all round, 36.8 × 0.3 = 11.04 m²; the top
//   is at 3.3.
// - HIP at 30° with eaves of 0.5 m: the eaves' ring is 11.2 × 9.2 = 103.04 m²; the ridge is 11.2 − 9.2 = 2 m long, at
//   3 + tan 30° (4.6 − 0.5); the roof's area is 103.04 / cos 30°; its edge hangs at 3 − 0.5 tan 30° all round; no wall rises.
// - GABLE, the west and east eaves standing: the ridge runs the whole 11.2 m at the same height; the same area; beneath each gable
//   the wall rises in a triangle of base 8.2 and height 4.1 tan 30°; the verge climbs to the ridge's height.
// - SHED at 10° falling south, eaves 0.3: the plane is at the plate over the south face and tan 10° × 8.2 above it over the north;
//   its area 10.8 × 8.8 / cos 10°; the north wall rises 10.2 × 8.2 tan 10°, the east and west walls half of 8.2 × 8.2 tan 10° each.
// - THE FACES: south 30 m² with a door; east 24 m² with 1.8 m² of glass; west 24 m² with 5.04 m².
// - THE GLASS. The west window, 2.4 × 2.1 from the floor, under the hip's eave (0.5 m out, its underside at 3 − 0.5 tan 30° − 0.1):
//   the Sun due west at 30° lights all of it; at 70°, the ray from a point at height z reaches the underside after
//   (2.6113 − z) / tan 70° and is under the eave when that is below 0.5, z > 1.2376: the top row (1.75) of three.
// - THE LIVING ROOM is 5.85 × 7.8 inside its walls under a ceiling at 2.7: surfaces 2 × 45.63 + 27.3 × 2.7 = 164.97 m²; with 5.04 m²
//   of glass seeing the whole sky (a flat roof, nothing about), its daylight factor is 0.68 × 5.04 × 90 / (164.97 × 0.75).
//
// AN L: [0, 8] × [0, 4] with [0, 4] × [4, 9] on centrelines, walls 0.2: outer faces 8.2 × 4.2 + 4.2 × 5.0 = 55.44 m². A hip with
// eaves 0.4: the eaves' ring is 9 × 5 + 5 × 5 = 70 m²; both arms are 5 m across at the eaves, so the ridges stand at
// plate + tan θ (2.5 − 0.4); there is a valley; the area is 70 / cos θ.
// A COURT: a house 12 × 12 with a court of 4 × 4 in its middle has two rings, the court's clockwise and moved into the court.

#include <architecture/shell/shell.hpp>

#include "../../plan/tests/house.hpp"

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>

namespace as = architecture::shell;
namespace ap = architecture::plan;
namespace ae = architecture::environment;
namespace cg = physis::computational_geometry;

namespace {

double length(const std::vector<as::Line>& lines, int kind) {
  double sum = 0;
  for (const auto& l : lines)
    if (l.kind == kind) sum += std::hypot(l.b[0] - l.a[0], l.b[1] - l.a[1]);
  return sum;
}

ap::Building ell() {
  ap::Building b;
  ap::Storey s;
  s.vertices = {{0, 0}, {8, 0}, {8, 4}, {4, 4}, {4, 9}, {0, 9}};
  for (std::size_t k = 0; k < 6; k++) s.walls.push_back(house::wall(k, (k + 1) % 6, 0.2));
  s.labels = {{{2, 2}, "living", "Living"}};
  b.storeys.push_back(s);
  return b;
}

ap::Building court() {
  ap::Building b;
  ap::Storey s;
  s.vertices = {{0, 0}, {12, 0}, {12, 12}, {0, 12}, {4, 4}, {8, 4}, {8, 8}, {4, 8}};
  for (std::size_t k = 0; k < 4; k++) s.walls.push_back(house::wall(k, (k + 1) % 4, 0.2));
  for (std::size_t k = 0; k < 4; k++) s.walls.push_back(house::wall(4 + k, 4 + (k + 1) % 4, 0.2));
  s.labels = {{{2, 6}, "living", "Living"}, {{6, 6}, "court", "Court"}};
  b.storeys.push_back(s);
  return b;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180;
  const auto b = house::building();
  const auto R = ap::read(b);

  // ---- the footprint ---------------------------------------------------------------------------------------------------------
  const auto prints = as::footprints(b.storeys[0], R.storeys[0]);
  s.require("the house has one footprint, of four edges, at its walls' outer faces", prints.size() == 1 && prints[0].ring.size() == 4 && prints[0].exact && !prints[0].court);
  s.check("... 10.2 × 8.2 = 83.64 m²", cg::ring_area(prints[0].ring) - 83.64, 1e-9, "m²");
  s.check("... on centrelines 80 m²", cg::ring_area(prints[0].centre) - 80, 1e-9, "m²");
  s.require("... the two walls of its south side are one edge", [&] { for (const auto& w : prints[0].walls) if (w.size() == 2 && ((w[0] == 0 && w[1] == 1) || (w[0] == 1 && w[1] == 0))) return true; return false; }());
  const as::Footprint& f = prints[0];

  // ---- the roof --------------------------------------------------------------------------------------------------------------
  {
    as::RoofSpec flat;
    flat.parapet = 0.3;
    const auto roof = as::roof(f, flat, 3.0);
    s.check("flat: the roof is the footprint", roof.area - 83.64, 1e-9, "m²");
    s.check("... the walls rise 0.3 m all round", roof.wall_area - 36.8 * 0.3, 1e-9, "m²");
    s.check("... to 3.3", roof.top - 3.3, 1e-12, "m");
  }
  const double t30 = std::tan(30 * deg);
  {
    as::RoofSpec hip;
    hip.form = "hip", hip.pitch = 30 * deg, hip.eaves = 0.5;
    const auto roof = as::roof(f, hip, 3.0);
    s.require("hip: exact, and no wall rises", roof.exact && roof.rises.empty());
    s.check("... the eaves' ring is 11.2 × 9.2", roof.plan_area - 103.04, 1e-7, "m²");
    s.check("... the roof's area is that over cos 30°", roof.area - 103.04 / std::cos(30 * deg), 1e-7, "m²");
    s.check("... the ridge is 2 m long", length(roof.lines, 0) - 2.0, 1e-7, "m");
    s.check("... at 3 + 4.1 tan 30°", roof.top - (3 + 4.1 * t30), 1e-9, "m");
    s.check("... four hips, each 4.6 √2 in plan", length(roof.lines, 1) - 4 * 4.6 * std::sqrt(2.0), 1e-6, "m");
    double worst = 0;
    for (const auto& p : roof.outline) worst = std::max(worst, std::fabs(p[2] - (3 - 0.5 * t30)));
    s.check("... its edge hangs at 3 − 0.5 tan 30° all round", worst, 1e-5, "m");
    worst = 0;
    for (const auto& face : roof.faces)
      for (const auto& p : face.ring)
        if (std::fabs(p[1] + 0.1) < 1e-9 || std::fabs(p[1] - 8.1) < 1e-9) worst = std::max(worst, std::fabs(p[2] - 3.0));
    s.check("... and it passes through the top of the walls' outer faces", worst, 1e-9, "m");
  }
  {
    as::RoofSpec gable;
    gable.form = "gable", gable.pitch = 30 * deg, gable.eaves = 0.5, gable.gables = {{-0.1, 4}, {10.1, 4}};
    const auto roof = as::roof(f, gable, 3.0);
    std::size_t made = 0;
    for (const char g : roof.gable) made += g ? 1 : 0;
    s.require("gable: the west and east eaves stand as gables", made == 2 && roof.refused == 0);
    s.check("... the ridge runs the whole 11.2 m", length(roof.lines, 0) - 11.2, 1e-7, "m");
    s.check("... at the hip's height", roof.top - (3 + 4.1 * t30), 1e-9, "m");
    s.check("... the same area", roof.area - 103.04 / std::cos(30 * deg), 1e-7, "m²");
    s.require("... two walls rise", roof.rises.size() == 2);
    s.check("... each in a triangle of base 8.2 and height 4.1 tan 30°", roof.rises.size() == 2 ? std::fabs(roof.rises[0].area - 0.5 * 8.2 * 4.1 * t30) + std::fabs(roof.rises[1].area - 0.5 * 8.2 * 4.1 * t30) : 1, 1e-7, "m²");
    s.require("... of three points: the foot, the apex at the middle, the foot", roof.rises.size() == 2 && roof.rises[0].top.size() == 3 && std::fabs(roof.rises[0].top[1].first - 4.1) < 1e-7 && std::fabs(roof.rises[0].top[1].second - (3 + 4.1 * t30)) < 1e-7);
    double high = 0;
    for (const auto& p : roof.outline) high = std::max(high, p[2]);
    s.check("... and the verge climbs to the ridge", high - (3 + 4.1 * t30), 1e-5, "m");
    as::RoofSpec far = gable;
    far.gables = {{5, 4}};
    s.require("a gable asked for far from every eave is not made: the roof is a hip", as::roof(f, far, 3.0).rises.empty());
  }
  {
    as::RoofSpec shed;
    shed.form = "shed", shed.pitch = 10 * deg, shed.eaves = 0.3, shed.fall = -90 * deg;
    const auto roof = as::roof(f, shed, 3.0);
    const double t10 = std::tan(10 * deg);
    s.check("shed: its area is 10.8 × 8.8 / cos 10°", roof.area - 10.8 * 8.8 / std::cos(10 * deg), 1e-9, "m²");
    s.check("... its top at the north eave, 3 + 8.5 tan 10°", roof.top - (3 + 8.5 * t10), 1e-9, "m");
    s.check("... the walls rise 10.2 × 8.2 tan 10° on the north and half of 8.2 × 8.2 tan 10° on the east and on the west", roof.wall_area - (10.2 * 8.2 * t10 + 8.2 * 8.2 * t10), 1e-9, "m²");
    s.require("... and not on the south", roof.rises.size() == 3);
  }

  // ---- the whole shell -------------------------------------------------------------------------------------------------------
  {
    as::RoofSpec hip;
    hip.form = "hip", hip.pitch = 30 * deg, hip.eaves = 0.5;
    const auto S = as::read(b, R, hip, {{30 * deg, 270 * deg, 1.0}, {70 * deg, 270 * deg, 1.0}});
    s.require("the house's shell: one roof on its one storey, from a plate at 3", S.roofs.size() == 1 && S.roofs[0].storey == 0 && S.roofs[0].plate == 3.0);
    s.check("the south face is 30 m² with a door and no glass", std::fabs(S.facades[6].wall - 30) + S.facades[6].glass + std::fabs(double(S.facades[6].doors) - 1), 1e-9, "");
    s.check("the east face 24 m² with 1.8 m² of glass", std::fabs(S.facades[0].wall - 24) + std::fabs(S.facades[0].glass - 1.8), 1e-9, "m²");
    s.check("the west face 24 m² with 5.04 m²", std::fabs(S.facades[4].wall - 24) + std::fabs(S.facades[4].glass - 5.04), 1e-9, "m²");
    s.require("two panes of glass", S.glass.size() == 2);
    const as::Glass* west = nullptr;
    for (const auto& g : S.glass)
      if (g.pane.nx < -0.5) west = &g;
    s.require("the west window is a pane on the wall's outer face, of the living room", west && std::fabs(west->pane.x + 0.1) < 1e-12 && std::fabs(west->pane.y - 4) < 1e-12 && R.storeys[0].rooms[west->room].kind == "living");
    s.check("... under the hip's eave, 0.5 m out with its underside at 3 − 0.5 tan 30° − 0.1", west ? std::fabs(west->pane.overhang - 0.5) + std::fabs(west->pane.overhang_z - (3 - 0.5 * t30 - 0.1)) : 1, 1e-12, "m");
    s.check("... lit wholly at 30° and but for its top row at 70°: 1 + 2/3 hours", west ? west->sun.hours - (1 + 2.0 / 3) : 1, 1e-12, "h");
    s.check("... its middle lit at both", west ? west->sun.full_hours - 2 : 1, 1e-12, "h");
    s.require("the shell knows it was given the Sun", S.lit);
  }
  {
    const auto S = as::read(b, R, as::RoofSpec{});
    const std::size_t living = house::room(R.storeys[0], "living");
    const as::RoomLight* room = nullptr;
    for (const auto& r : S.rooms)
      if (r.room == living) room = &r;
    s.check("the living room's surfaces are 2 × 45.63 + 27.3 × 2.7 m²", room ? room->surfaces - 164.97 : 1, 1e-9, "m²");
    s.check("... its daylight factor 0.68 × 5.04 × 90 / (164.97 × 0.75)", room ? room->daylight_factor - 0.68 * 5.04 * 90 / (164.97 * 0.75) : 1, 1e-9, "%");
    s.require("with no Sun given, no hours and no gain are counted", !S.lit && S.solar_gain == 0);
    // A neighbour 6 m high standing 10 m west of the wall hides the sky to atan((6 − 1.05) / 9.95) from the pane's middle.
    const auto T = as::read(b, R, as::RoofSpec{}, {}, {ae::Prism{{{-12, -20}, {-10.1, -20}, {-10.1, 20}, {-12, 20}}, 6.0, -1}});
    for (const auto& r : T.rooms)
      if (r.room == living) room = &r;
    s.check("a neighbour of 6 m at 10 m leaves the living room's glass 90° − atan(4.95 / 9.95) of sky", room->sky - (std::numbers::pi / 2 - std::atan(4.95 / 9.95)), 1e-9, "rad");
  }
  {
    // Two storeys: the roof stands on the upper, from 6 m.
    ap::Building two = b;
    two.storeys.push_back(house::storey());
    const auto S = as::read(two, ap::read(two), as::RoofSpec{});
    s.require("of two storeys, the roof stands on the upper, from a plate at 6", S.roofs.size() == 1 && S.roofs[0].storey == 1 && S.roofs[0].plate == 6.0);
    s.check("... and the faces are twice as high, with the parapet's 0.3 m above them", S.facades[6].wall - (60 + 10.2 * 0.3), 1e-9, "m²");
  }

  // ---- an L, and a court -----------------------------------------------------------------------------------------------------
  {
    const auto l = ell();
    const auto Rl = ap::read(l);
    const auto fl = as::footprints(l.storeys[0], Rl.storeys[0]);
    s.require("the L has one footprint of six edges", fl.size() == 1 && fl[0].ring.size() == 6 && fl[0].exact);
    s.check("... 55.44 m² at its outer faces", fl.size() == 1 ? cg::ring_area(fl[0].ring) - 55.44 : 1, 1e-9, "m²");
    as::RoofSpec hip;
    hip.form = "hip", hip.pitch = 25 * deg, hip.eaves = 0.4;
    const auto roof = as::roof(fl[0], hip, 3.0);
    s.check("its hip's eaves ring is 70 m²", roof.plan_area - 70, 1e-6, "m²");
    s.check("... its area 70 / cos 25°", roof.area - 70 / std::cos(25 * deg), 1e-6, "m²");
    s.check("... its ridges at 3 + 2.1 tan 25°", roof.top - (3 + 2.1 * std::tan(25 * deg)), 1e-7, "m");
    s.check("... with one valley, from the inner corner to the ridges' meeting: 2.5 √2 in plan", length(roof.lines, 2) - 2.5 * std::sqrt(2.0), 1e-6, "m");
    as::RoofSpec gable = hip;
    gable.form = "gable", gable.gables = {{8.1, 2}, {2, 9.1}, {4.1, 6}};
    const auto g = as::roof(fl[0], gable, 3.0);
    std::size_t made = 0;
    for (const char e : g.gable) made += e ? 1 : 0;
    s.require("gables stand at the ends of its two arms; one asked for on the inner side is refused, and said to be", made == 2 && g.refused == 1 && !g.note.empty());
    s.check("... the area is still 70 / cos 25°", g.area - 70 / std::cos(25 * deg), 1e-6, "m²");
  }
  {
    const auto c = court();
    const auto Rc = ap::read(c);
    const auto fc = as::footprints(c.storeys[0], Rc.storeys[0]);
    s.require("a house about a court has two rings, one of them the court's", fc.size() == 2 && (fc[0].court != fc[1].court));
    for (const auto& ring : fc) {
      if (ring.court) s.check("... the court's ring runs clockwise, at the walls' faces within the court: 3.8 × 3.8", cg::ring_area(ring.ring) + 3.8 * 3.8, 1e-9, "m²");
      else s.check("... the outer ring 12.2 × 12.2", cg::ring_area(ring.ring) - 12.2 * 12.2, 1e-9, "m²");
    }
  }
  return s.done();
}
