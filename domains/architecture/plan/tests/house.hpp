// A house small enough to work by hand, for the checks of plan, quantities, habitability and programme.
//
// 10 × 8 m to the centrelines of its outer walls (0.2 m thick), divided by one wall 0.1 m thick at x = 6 into a living room on the
// west (6 × 8) and a bedroom on the east (4 × 8). An entrance 0.9 wide on the south wall of the living room, a door 0.82 wide between
// the rooms, a window 1.5 × 1.2 on the bedroom's east wall and one 2.4 × 2.1 on the living room's west wall, each half openable. The
// storey is 3.0 m floor to floor with a ceiling at 2.7 m.

#pragma once

#include <architecture/plan/plan.hpp>

#include <string>

namespace house {

namespace ap = architecture::plan;

inline ap::Wall wall(std::size_t a, std::size_t b, double t, ap::WallKind kind = ap::WallKind::solid) {
  ap::Wall w;
  w.a = a, w.b = b, w.thickness = t, w.kind = kind;
  return w;
}

inline ap::Opening opening(std::size_t on, ap::OpeningKind kind, double at, double width, double height, double sill, double glazed, double openable) {
  ap::Opening o;
  o.wall = on, o.kind = kind, o.at = at, o.width = width, o.height = height, o.sill = sill, o.glazed = glazed, o.openable = openable;
  return o;
}

inline ap::Storey storey() {
  ap::Storey s;
  s.name = "ground";
  s.vertices = {{0, 0}, {6, 0}, {10, 0}, {10, 8}, {6, 8}, {0, 8}};
  s.walls = {wall(0, 1, 0.2), wall(1, 2, 0.2), wall(2, 3, 0.2), wall(3, 4, 0.2), wall(4, 5, 0.2), wall(5, 0, 0.2), wall(1, 4, 0.1)};
  s.labels = {{{3, 4}, "living", "Living"}, {{8, 4}, "bedroom", "Bedroom"}};
  s.openings = {opening(0, ap::OpeningKind::door, 3, 0.9, 2.04, 0, 0, 1), opening(6, ap::OpeningKind::door, 4, 0.82, 2.04, 0, 0, 1),
                opening(2, ap::OpeningKind::window, 4, 1.5, 1.2, 0.9, 1, 0.5), opening(5, ap::OpeningKind::window, 4, 2.4, 2.1, 0, 1, 0.5)};
  s.height = 3.0, s.ceiling = 2.7;
  return s;
}

inline ap::Building building() {
  ap::Building b;
  b.storeys.push_back(storey());
  return b;
}

// The room of a kind on a storey; none when there is not exactly one.
inline std::size_t room(const ap::StoreyReading& R, const std::string& kind) {
  std::size_t found = ap::none;
  for (std::size_t r = 0; r < R.rooms.size(); r++)
    if (R.rooms[r].kind == kind) {
      if (found != ap::none) return ap::none;
      found = r;
    }
  return found;
}

}  // namespace house
