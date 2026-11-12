// Checks of a building as it is drawn (plan.hpp), on plans worked by hand.
//
// - The house of house.hpp: its two rooms are the bounded faces (V − E + F = 2), 48 and 32 m² to the centrelines; inside the walls'
//   faces 5.85 × 7.8 and 3.85 × 7.8; their widths the lesser sides; each wall's two sides; the outer walls.
// - Light and air: the glass and the openable area on each room's outer walls.
// - Access: outside – living – bedroom is a chain, so the depths are 1 and 2, the bedroom is reached through the living room, and
//   the relative asymmetries are 1, 0, 1.
// - What stands free in a room is a hole of it: a wall that divides nothing, and a core that is a room of its own.
// - An L-shaped room: its net area by the mitred inset, A − d P + d² Σ tan(φ/2) with five convex corners and one reflex.
// - A storey that is not a plane graph is refused, each fault named: walls that cross, a wall through a vertex, a wall twice.
// - Open walls join spaces without a door; a stair joins two storeys; a court's walls are outer walls.

#include "house.hpp"

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>

namespace ap = architecture::plan;

namespace {
bool says(const std::vector<std::string>& lines, const std::string& part) {
  return std::any_of(lines.begin(), lines.end(), [&](const std::string& l) { return l.find(part) != std::string::npos; });
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- the house -------------------------------------------------------------------------------------------------------------
  {
    const auto st = house::storey();
    const auto R = ap::read(st);
    s.require("the house is a plane graph, and its rooms are two", R.sound && R.rooms.size() == 2);
    s.require("... Euler's formula: V − E + F = 2 with the outside a face", static_cast<int>(st.vertices.size()) - static_cast<int>(st.walls.size()) + static_cast<int>(R.rooms.size()) + 1 == 2);
    const std::size_t living = house::room(R, "living"), bedroom = house::room(R, "bedroom");
    s.require("... the labels name them", living != ap::none && bedroom != ap::none && R.rooms[living].name == "Living");
    s.check("the living room is 6 × 8 to the centrelines", R.rooms[living].area - 48, 1e-12, "m²");
    s.check("the bedroom is 4 × 8", R.rooms[bedroom].area - 32, 1e-12, "m²");
    s.check("the rooms tile the outline: 80 m² of floor", R.floor_area - 80, 1e-12, "m²");
    s.check("inside its walls' faces the living room is 5.85 × 7.8", R.rooms[living].net_area - 5.85 * 7.8, 1e-12, "m²");
    s.check("... and the bedroom 3.85 × 7.8", R.rooms[bedroom].net_area - 3.85 * 7.8, 1e-12, "m²");
    s.require("... both exactly, by the mitred inset", R.rooms[living].net_exact && R.rooms[bedroom].net_exact);
    s.check("... and the internal area is their sum", R.internal_area - (5.85 + 3.85) * 7.8, 1e-12, "m²");
    s.check("the living room's width is its lesser side, 5.85 m", R.rooms[living].width - 5.85, 1e-6, "m");
    s.check("the bedroom's width is 3.85 m", R.rooms[bedroom].width - 3.85, 1e-6, "m");
    s.require("the dividing wall has the living room on its left and the bedroom on its right", R.sides[6].left == living && R.sides[6].right == bedroom && !R.sides[6].outer);
    s.require("the six others are outer walls, one side the outside",
              std::all_of(R.sides.begin(), R.sides.begin() + 6, [](const ap::Side& x) { return x.outer && (x.left == ap::none) != (x.right == ap::none); }));
    s.check("the living room has 20 m of outer wall, the bedroom 16", std::fabs(R.rooms[living].outer_length - 20) + std::fabs(R.rooms[bedroom].outer_length - 16), 1e-12, "m");
    s.check("the bedroom's window is 1.8 m² of glass", R.rooms[bedroom].glazed_area - 1.8, 1e-12, "m²");
    s.check("... of which 0.9 m² opens", R.rooms[bedroom].openable_area - 0.9, 1e-12, "m²");
    s.check("the living room's glass is its window's 5.04 m²: the entrance is not glazed", R.rooms[living].glazed_area - 5.04, 1e-12, "m²");
    s.check("... and it is aired by half the window and the whole entrance", R.rooms[living].openable_area - (2.52 + 0.9 * 2.04), 1e-12, "m²");
    s.require("no remark is made of it", R.remarks.empty());

    ap::Building b;
    b.storeys.push_back(st);
    const auto B = ap::read(b);
    const std::size_t vl = B.vertex(0, living), vb = B.vertex(0, bedroom);
    s.require("the living room is one step from the outside, the bedroom two", B.depth[vl] == 1 && B.depth[vb] == 2);
    s.require("the bedroom is reached through the living room, the living room through nothing",
              B.through[vb] == std::vector<std::size_t>{vl} && B.through[vl].empty());
    s.check("on a chain of three the relative asymmetries are 1, 0, 1", std::fabs(B.asymmetry[0] - 1) + std::fabs(B.asymmetry[vl]) + std::fabs(B.asymmetry[vb] - 1), 1e-12);

    // Without its entrance, nothing is reached.
    ap::Building shut = b;
    shut.storeys[0].openings.erase(shut.storeys[0].openings.begin());
    const auto S = ap::read(shut);
    s.require("without the entrance no room is reached", !std::isfinite(S.depth[vl]) && !std::isfinite(S.depth[vb]));

    // An open wall in place of the dividing wall joins the rooms, and takes nothing from their floors.
    ap::Building open = shut;
    open.storeys[0].walls[6].kind = ap::WallKind::open;
    open.storeys[0].openings.erase(open.storeys[0].openings.begin());   // the door that stood on it
    open.storeys[0].openings.push_back(house::opening(0, ap::OpeningKind::door, 3, 0.9, 2.04, 0, 0, 1));
    const auto O = ap::read(open);
    s.require("an open wall joins the two rooms without a door", O.depth[vl] == 1 && O.depth[vb] == 2);
    s.check("... and the living room keeps the 0.05 × 7.8 the wall took", O.storeys[0].rooms[living].net_area - 5.9 * 7.8, 1e-12, "m²");
  }

  // ---- what stands free in a room --------------------------------------------------------------------------------------------
  {
    auto st = house::storey();
    // A wall of 1 m standing free in the living room, and a core 1 × 1 in the bedroom.
    st.vertices.insert(st.vertices.end(), {{2, 3}, {3, 3}, {7.5, 3.5}, {8.5, 3.5}, {8.5, 4.5}, {7.5, 4.5}});
    st.walls.push_back(house::wall(6, 7, 0.1));
    for (std::size_t k = 0; k < 4; k++) st.walls.push_back(house::wall(8 + k, 8 + (k + 1) % 4, 0.1));
    st.labels.push_back({{8, 4}, "store", "Core"});
    st.labels[1].at = {9.5, 1};   // the bedroom's label, moved out of the core
    const auto R = ap::read(st);
    const std::size_t living = house::room(R, "living"), bedroom = house::room(R, "bedroom"), core = house::room(R, "store");
    s.require("a free wall and a core leave three rooms", R.sound && R.rooms.size() == 3 && core != ap::none);
    s.require("the free wall divides nothing: the living room on both its sides", R.sides[7].divides_nothing && R.sides[7].left == living && R.sides[7].right == living);
    s.check("... and the living room keeps its 48 m²", R.rooms[living].area - 48, 1e-12, "m²");
    s.check("... its net area to the first order: 48 less half the thickness along each wall, the free wall's two faces among them",
            R.rooms[living].net_area - (48 - 0.6 - 0.4 - 0.6 - 0.8 - 0.1), 1e-12, "m²");
    s.require("... and said to be approximate", !R.rooms[living].net_exact);
    s.check("the core is a room of 1 m², and a hole of the bedroom: 31 m²", std::fabs(R.rooms[core].area - 1) + std::fabs(R.rooms[bedroom].area - 31), 1e-12, "m²");
    s.require("each wall of the core has the core on one side and the bedroom on the other",
              std::all_of(R.sides.begin() + 8, R.sides.end(), [&](const ap::Side& x) { return (x.left == core && x.right == bedroom) || (x.left == bedroom && x.right == core); }));
    s.check("the floor is still 80 m²", R.floor_area - 80, 1e-12, "m²");
  }

  // ---- an L-shaped room ------------------------------------------------------------------------------------------------------
  {
    ap::Storey st;
    st.vertices = {{0, 0}, {6, 0}, {6, 3}, {3, 3}, {3, 6}, {0, 6}};
    for (std::size_t k = 0; k < 6; k++) st.walls.push_back(house::wall(k, (k + 1) % 6, 0.2));
    const auto R = ap::read(st);
    s.require("an L is one room", R.sound && R.rooms.size() == 1 && R.rooms[0].simple);
    s.check("... of 27 m²", R.rooms[0].area - 27, 1e-12, "m²");
    s.check("... and 27 − 0.1 × 24 + 0.01 × 4 inside its walls: five convex corners and one reflex", R.rooms[0].net_area - (27 - 2.4 + 0.04), 1e-12, "m²");
    s.check("... its width that of its arms, 2.8 m", R.rooms[0].width - 2.8, 1e-5, "m");
  }

  // ---- a court ---------------------------------------------------------------------------------------------------------------
  {
    auto st = house::storey();
    st.labels[1].kind = "court";
    const auto R = ap::read(st);
    const std::size_t living = house::room(R, "living");
    s.require("where the bedroom is a court, the wall between is an outer wall", R.sides[6].outer);
    s.check("... the floor is the living room's 48 m²", R.floor_area - 48, 1e-12, "m²");
    s.check("... which now has 28 m of outer wall", R.rooms[living].outer_length - 28, 1e-12, "m");
  }

  // ---- what is refused -------------------------------------------------------------------------------------------------------
  {
    auto st = house::storey();
    st.walls.push_back(house::wall(0, 3, 0.1));   // a diagonal across the dividing wall
    const auto R = ap::read(st);
    s.require("walls that cross are refused, and named", !R.sound && R.rooms.empty() && says(R.faults, "cross") && R.faulty_walls.size() == 2);
  }
  {
    auto st = house::storey();
    st.vertices.push_back({3, 8});
    st.vertices.push_back({3, 5});
    st.walls.push_back(house::wall(6, 7, 0.1));   // it starts on the north wall without splitting it
    const auto R = ap::read(st);
    s.require("a wall through a vertex that does not end there is refused", !R.sound && says(R.faults, "passes through a vertex"));
  }
  {
    auto st = house::storey();
    st.walls.push_back(house::wall(4, 1, 0.1));
    const auto R = ap::read(st);
    s.require("a wall drawn twice is refused", !R.sound && says(R.faults, "the same wall"));
  }
  {
    auto st = house::storey();
    st.openings.push_back(house::opening(6, ap::OpeningKind::window, 2, 1, 1, 1, 1, 0));
    st.openings.push_back(house::opening(0, ap::OpeningKind::door, 5.8, 0.9, 2.04, 0, 0, 1));
    st.labels.push_back({{1, 1}, "study", ""});
    const auto R = ap::read(st);
    s.require("a window between two rooms, an opening past its wall's end and a second label are remarked, and the rooms still read",
              R.sound && R.rooms.size() == 2 && says(R.remarks, "between two rooms") && says(R.remarks, "beyond the ends") && says(R.remarks, "second label"));
  }

  // ---- two storeys and a stair -----------------------------------------------------------------------------------------------
  {
    ap::Building b = house::building();
    ap::Storey up;
    up.vertices = {{0, 0}, {6, 0}, {6, 8}, {0, 8}};
    for (std::size_t k = 0; k < 4; k++) up.walls.push_back(house::wall(k, (k + 1) % 4, 0.2));
    up.labels = {{{3, 4}, "bedroom", "Upper bedroom"}};
    b.storeys.push_back(up);
    ap::Stair stair;
    stair.from = 0, stair.at = {1, 1}, stair.bearing = std::numbers::pi / 2, stair.width = 1.0, stair.going = 0.25, stair.risers = 17;
    b.stairs.push_back(stair);
    const auto B = ap::read(b);
    s.require("a stair from the living room joins the room above: its depth is 2", B.sound && B.depth[B.vertex(1, 0)] == 2);
    s.check("... its riser is the storey's 3 m over 17", B.stairs[0].riser - 3.0 / 17, 1e-15, "m");
    s.check("... and its run 16 goings", B.stairs[0].run - 4.0, 1e-12, "m");
  }
  return s.done();
}
