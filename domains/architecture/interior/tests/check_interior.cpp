// Checks of what stands in a room (interior.hpp), in a bedroom of 4 × 3 m inside its walls, worked by hand.
//
// The room: walls 0.2 m thick on the centrelines of a box 4.2 × 3.2 m, so that its net ring is [0.1, 4.1] × [0.1, 3.1]. A door 0.8 m
// wide in the south wall, from x = 0.3 to 1.1, swinging in. A double bed 1.6 × 2.0 with its head to the north wall, its foot needing
// 0.6 m and each side 0.5 m; a wardrobe 1.0 × 0.6 against the west wall, needing 0.8 m before it.
//
// - Geometry: a piece's footprint and spaces by its bearing; the area two rectangles share, against areas known by hand.
// - Laid out so, every piece is within the room, apart from the others, its space free, and reached; pieces' spaces may overlap.
// - The wardrobe moved into the bed's side space blocks the bed, and the bed blocks the wardrobe; moved onto the bed, they overlap;
//   moved through the wall, it is not within the room; flush against the wall's face, it is.
// - A chair behind the door stands in the square its leaf sweeps; drawn to swing the other way, the door is clear.
// - A counter across the room leaving 0.8 m lets a person of 0.6 m pass to what is beyond; leaving 0.4 m, it does not.
// - The free share of the floor; a rug obstructs nothing; a piece in no room is named so.

#include <architecture/interior/interior.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>

namespace ai = architecture::interior;
namespace ap = architecture::plan;

namespace {

ap::Building bedroom() {
  ap::Building b;
  ap::Storey s;
  s.vertices = {{0, 0}, {4.2, 0}, {4.2, 3.2}, {0, 3.2}};
  for (std::size_t k = 0; k < 4; k++) {
    ap::Wall w;
    w.a = k, w.b = (k + 1) % 4, w.thickness = 0.2;
    s.walls.push_back(w);
  }
  ap::Opening door;
  door.wall = 0, door.kind = ap::OpeningKind::door, door.at = 0.7, door.width = 0.8;
  s.openings.push_back(door);
  s.labels = {{{2, 1.5}, "bedroom", "Bedroom"}};
  b.storeys.push_back(s);
  return b;
}

ai::Piece piece(const char* kind, double x, double y, double degrees, double width, double depth, double front = 0, double side = 0, bool obstructs = true) {
  ai::Piece p;
  p.kind = kind, p.at = {x, y}, p.bearing = degrees * std::numbers::pi / 180, p.width = width, p.depth = depth, p.front = front, p.side = side, p.obstructs = obstructs;
  return p;
}

double span(const ai::Ring& r, int axis, bool high) {
  double v = r[0][axis];
  for (const auto& p : r) v = high ? std::max(v, p[axis]) : std::min(v, p[axis]);
  return v;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto b = bedroom();
  const auto R = ap::read(b);
  s.check("the bedroom is 4 × 3 m inside its walls", R.storeys[0].rooms[0].net_area - 12, 1e-12, "m²");

  // ---- geometry --------------------------------------------------------------------------------------------------------------
  const auto bed = piece("bed", 2.5, 2.1, -90, 1.6, 2.0, 0.6, 0.5);         // its front faces south: its head is to the north wall
  const auto wardrobe = piece("wardrobe", 0.4, 2.3, 0, 1.0, 0.6, 0.8);     // its front faces east: its back is to the west wall
  {
    const auto f = ai::footprint(bed);
    s.check("a bed 1.6 wide and 2.0 deep, facing south at (2.5, 2.1), stands on [1.7, 3.3] × [1.1, 3.1]",
            std::fabs(span(f, 0, false) - 1.7) + std::fabs(span(f, 0, true) - 3.3) + std::fabs(span(f, 1, false) - 1.1) + std::fabs(span(f, 1, true) - 3.1), 1e-12, "m");
    s.check("... its footprint counter-clockwise, of 3.2 m²", physis::computational_geometry::ring_area(f) - 3.2, 1e-12, "m²");
    const auto sp = ai::spaces(bed);
    s.require("it needs three spaces: its foot and its two sides", sp.size() == 3);
    s.check("... the foot's 0.6 m lies south of it, on [1.7, 3.3] × [0.5, 1.1]",
            std::fabs(span(sp[0], 0, false) - 1.7) + std::fabs(span(sp[0], 0, true) - 3.3) + std::fabs(span(sp[0], 1, false) - 0.5) + std::fabs(span(sp[0], 1, true) - 1.1), 1e-12, "m");
    s.check("... and the sides' 0.5 m on x in [1.2, 1.7] and [3.3, 3.8]",
            std::fabs(std::min(span(sp[1], 0, false), span(sp[2], 0, false)) - 1.2) + std::fabs(std::max(span(sp[1], 0, true), span(sp[2], 0, true)) - 3.8), 1e-12, "m");
    const auto w = ai::footprint(wardrobe);
    s.check("a wardrobe 1.0 wide and 0.6 deep, facing east at (0.4, 2.3), stands on [0.1, 0.7] × [1.8, 2.8]",
            std::fabs(span(w, 0, false) - 0.1) + std::fabs(span(w, 0, true) - 0.7) + std::fabs(span(w, 1, false) - 1.8) + std::fabs(span(w, 1, true) - 2.8), 1e-12, "m");
    const ai::Ring unit{{0, 0}, {1, 0}, {1, 1}, {0, 1}}, moved{{0.5, 0}, {1.5, 0}, {1.5, 1}, {0.5, 1}}, apart{{1, 0}, {2, 0}, {2, 1}, {1, 1}};
    const ai::Ring diamond{{0.5, -0.5}, {1.5, 0.5}, {0.5, 1.5}, {-0.5, 0.5}};
    s.check("two unit squares half a metre apart share 0.5 m²", ai::shared_area(unit, moved) - 0.5, 1e-12, "m²");
    s.check("... two that touch along an edge share nothing", ai::shared_area(unit, apart), 1e-12, "m²");
    s.check("... and a unit square shares all of itself with the diamond of diagonal 2 about its middle", ai::shared_area(unit, diamond) - 1, 1e-12, "m²");
  }

  // ---- laid out well ---------------------------------------------------------------------------------------------------------
  {
    const auto I = ai::read(b, R, {bed, wardrobe});
    s.require("laid out so, the bed and the wardrobe are within the room, apart, their spaces free, and reached", I.pieces[0].ok() && I.pieces[1].ok() && I.pieces[0].judged);
    s.require("... though the wardrobe's space and the bed's west side overlap: one stands in each in turn", ai::shared_area(ai::spaces(wardrobe)[0], ai::spaces(bed)[1]) + ai::shared_area(ai::spaces(wardrobe)[0], ai::spaces(bed)[2]) > 0.2);
    s.require("the door's swing is clear and a person can stand behind it", I.doors.size() == 1 && I.doors[0].swing_blocked_by.empty() && I.doors[0].entered);
    s.check("of the 12 m² of floor, the bed's 3.2 and the wardrobe's 0.6 leave 68.3% free, to the grid", I.rooms[0].free_share - (1 - 3.8 / 12), 0.01);
  }

  // ---- laid out badly --------------------------------------------------------------------------------------------------------
  {
    auto w = wardrobe;
    w.at = {1.3, 2.3};   // on [1.0, 1.6]: in the bed's west side space, its own space reaching the bed
    const auto I = ai::read(b, R, {bed, w});
    s.require("the wardrobe moved beside the bed stands in the bed's side space, and the bed in the wardrobe's: each blocks the other",
              I.pieces[0].blocked_by == std::vector<std::size_t>{1} && I.pieces[1].blocked_by == std::vector<std::size_t>{0});
    s.require("... but they do not overlap, and both are within the room", I.pieces[0].overlaps.empty() && I.pieces[1].overlaps.empty() && I.pieces[0].within && I.pieces[1].within);
  }
  {
    auto w = wardrobe;
    w.at = {2.0, 2.3};
    const auto I = ai::read(b, R, {bed, w});
    s.require("moved onto the bed, each overlaps the other", I.pieces[0].overlaps == std::vector<std::size_t>{1} && I.pieces[1].overlaps == std::vector<std::size_t>{0} && !I.pieces[0].ok());
  }
  {
    auto w = wardrobe;
    w.at = {0.2, 2.3};   // on [−0.1, 0.5]: through the west wall's face at 0.1
    const auto I = ai::read(b, R, {bed, w});
    s.require("moved through the wall's face, the wardrobe is not within the room; the bed still is", !I.pieces[1].within && I.pieces[0].within);
    s.require("... while flush against the face, as first laid out, it is", ai::read(b, R, {bed, wardrobe}).pieces[1].within);
    auto east = bed;
    east.at = {3.3, 2.1};   // against the east wall: its east side space would lie in the wall
    s.require("a double bed pushed against a wall has no room at that side: its space is not within the room", !ai::read(b, R, {east}).pieces[0].space_within && ai::read(b, R, {east}).pieces[0].within);
  }

  // ---- the door --------------------------------------------------------------------------------------------------------------
  {
    const auto chair = piece("chair", 0.7, 0.5, 90, 0.5, 0.5);
    const auto I = ai::read(b, R, {bed, wardrobe, chair});
    s.require("a chair behind the door stands in the square its leaf sweeps", I.doors[0].swing_blocked_by == std::vector<std::size_t>{2});
    auto out = b;
    out.storeys[0].openings[0].swing = -1;
    s.require("... and with the door drawn to swing outward, nothing is in its way", ai::read(out, ap::read(out), {bed, wardrobe, chair}).doors[0].swing_blocked_by.empty());
    const auto chest = piece("chest", 0.7, 0.75, 90, 1.2, 1.1);   // on [0.1, 1.3] × [0.2, 1.3]: the whole floor behind the door
    const auto J = ai::read(b, R, {chest});
    s.require("a chest filling the floor behind the door leaves no one room to stand there: the door cannot be entered", !J.doors[0].entered && !J.doors[0].swing_blocked_by.empty());
  }

  // ---- reaching --------------------------------------------------------------------------------------------------------------
  {
    const auto table = piece("table", 2.0, 2.5, -90, 0.8, 0.6);
    const auto wide = piece("counter", 1.7, 1.5, -90, 3.2, 0.6);     // on [0.1, 3.3] × [1.2, 1.8]: 0.8 m left at its east end
    const auto wider = piece("counter", 1.9, 1.5, -90, 3.6, 0.6);    // on [0.1, 3.7]: 0.4 m left
    const auto I = ai::read(b, R, {wide, table});
    s.require("a counter across the room that leaves 0.8 m lets a person of 0.6 m pass: the table beyond is reached", I.pieces[1].judged && I.pieces[1].reached && I.pieces[0].reached);
    const auto J = ai::read(b, R, {wider, table});
    s.require("... and one that leaves 0.4 m does not: the table is not reached, though it is within the room and apart", !J.pieces[1].reached && J.pieces[1].within && J.pieces[1].overlaps.empty() && J.pieces[0].reached);
    s.require("... while a person of 0.3 m passes it", ai::read(b, R, {wider, table}, 0.3).pieces[1].reached);
    const auto bench = piece("bench", 3.9, 2.8, 180, 0.6, 0.4, 0.6);   // on [3.7, 4.1] × [2.5, 3.1]: in the north-east corner, facing west
    s.require("a piece is reached where its space can be stood in: a bench beyond the wide gap, and not beyond the narrow one",
              ai::read(b, R, {wide, bench}).pieces[1].reached && !ai::read(b, R, {wider, bench}).pieces[1].reached);
  }

  // ---- what obstructs nothing, and what is nowhere ---------------------------------------------------------------------------
  {
    const auto rug = piece("rug", 2.5, 1.6, 0, 2.4, 1.6, 0, 0, false);
    const auto I = ai::read(b, R, {bed, rug});
    s.require("a rug under the bed obstructs nothing: neither overlaps the other, and the floor's free share is the bed's alone",
              I.pieces[0].ok() && I.pieces[1].ok() && std::fabs(I.rooms[0].free_share - (1 - 3.2 / 12)) < 0.01);
    const auto lost = piece("chair", 9, 9, 0, 0.5, 0.5);
    const auto J = ai::read(b, R, {lost});
    s.require("a piece whose middle lies in no room is in none, and is not well placed", J.pieces[0].room == ap::none && !J.pieces[0].ok());
    s.require("an empty room is all free floor, and nothing is judged of reaching", ai::read(b, R, {}).rooms[0].free_share == 1.0 && ai::read(b, R, {}).pieces.empty());
  }
  return s.done();
}
