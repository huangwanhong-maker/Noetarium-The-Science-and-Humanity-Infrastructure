// Whether a drawn building can be lived in, rule by rule (ADR 0095): what a building code states of ceilings, light, air, doors,
// passages and stairs, found of each room, door and stair.
//
// THE CODE states limits (rules' finding.hpp); a limit not stated is not evaluated, and none here is any jurisdiction's. Its forms:
// - a ceiling height, at least: for habitable rooms, for the others, and for a kind by name where the code gives it one;
// - natural light: the glazed area on a room's outer walls as a share of its floor, at least, asked of habitable rooms;
// - ventilation: the openable area on its outer walls as a share of its floor, at least, asked of habitable rooms;
// - the clear width of a door, at least: of the doors to the outside (an entrance), and of the others;
// - the clear width of a passage, at least: the width of a hall, corridor, entry or landing (plan.hpp: for a rectangle its lesser
//   side; for a passage that turns, its wider arm's, so a narrow arm is not found);
// - a stair's riser R, its going G and the slope relationship 2R + G, each within a range, and the risers of a flight within a range;
// - a floor area and a width, at least, for a kind by name.
// The floor a share is taken of is the room's net area (inside the walls' faces). A code may state a rule this drawing cannot
// answer (the light a window gets from a court of a given size, say); such rules are not here.

#pragma once

#include <architecture/plan/plan.hpp>
#include <architecture/rules/finding.hpp>

#include <map>
#include <string>
#include <vector>

namespace architecture::plan {

using rules::Finding;
using rules::Limit;

struct Code {
  std::string name, version, source;
  Limit ceiling_habitable, ceiling_other;
  std::map<std::string, Limit> ceiling;   // by kind, in place of the two above
  Limit light, air;
  Limit entrance, door;
  Limit passage;
  Limit riser, going, slope, flight;
  std::map<std::string, Limit> area, width;   // by kind
};

inline std::vector<Finding> check(const Building& b, const Reading& R, const Code& c) {
  std::vector<Finding> out;
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    if (!S.sound) continue;
    const std::string where = b.storeys.size() > 1 ? " (" + (s.name.empty() ? "storey " + std::to_string(k + 1) : s.name) + ")" : "";
    for (const auto& room : S.rooms) {
      const Use use = use_of(room.kind);
      if (!use.indoor) continue;
      const std::string subject = called(room) + where;
      const auto by_kind = c.ceiling.find(room.kind);
      const Limit& ceiling = by_kind != c.ceiling.end() ? by_kind->second : use.habitable ? c.ceiling_habitable : c.ceiling_other;
      if (ceiling.stated()) out.push_back(rules::found("ceiling", "Ceiling height", subject, ceiling, s.ceiling, "m"));
      if (use.habitable && room.net_area > 0) {
        if (c.light.stated()) out.push_back(rules::found("light", "Natural light", subject, c.light, room.glazed_area / room.net_area, "share"));
        if (c.air.stated()) out.push_back(rules::found("air", "Ventilation", subject, c.air, room.openable_area / room.net_area, "share"));
      }
      if (use.circulation && c.passage.stated() && !std::isnan(room.width)) out.push_back(rules::found("passage", "Clear width of a passage", subject, c.passage, room.width, "m", room.net_ring.size() > 4 ? "of its widest part; a narrower arm is not found" : "", 1e-6));
      if (const auto a = c.area.find(room.kind); a != c.area.end() && a->second.stated())
        out.push_back(rules::found("area", "Floor area", subject, a->second, room.net_area, "m²", room.net_exact ? "" : "to the first order in the walls' thickness"));
      if (const auto w = c.width.find(room.kind); w != c.width.end() && w->second.stated() && !std::isnan(room.width))
        out.push_back(rules::found("width", "Least width", subject, w->second, room.width, "m", "the largest circle the room holds", 1e-6));
    }
    for (std::size_t i = 0; i < s.openings.size(); i++) {
      const auto& o = s.openings[i];
      if (o.kind != OpeningKind::door || o.wall >= S.sides.size()) continue;
      const auto& side = S.sides[o.wall];
      const bool to_outside = side.left == none || side.right == none;
      const Limit& need = to_outside ? c.entrance : c.door;
      if (!need.stated()) continue;
      const std::size_t r = side.left != none ? side.left : side.right;
      const std::string of = r != none ? called(S.rooms[r]) : "nothing";
      out.push_back(rules::found(to_outside ? "entrance" : "door", to_outside ? "Clear width of an entrance" : "Clear width of a door",
                                 "door " + std::to_string(i + 1) + " of " + of + where, need, o.width, "m"));
    }
  }
  for (std::size_t i = 0; i < b.stairs.size() && i < R.stairs.size(); i++) {
    const auto& st = b.stairs[i];
    const auto& sr = R.stairs[i];
    if (!(sr.riser > 0)) continue;
    const std::string subject = "stair " + std::to_string(i + 1);
    if (c.riser.stated()) out.push_back(rules::found("riser", "Stair: riser", subject, c.riser, sr.riser, "m", "", 1e-6));
    if (c.going.stated()) out.push_back(rules::found("going", "Stair: going", subject, c.going, st.going, "m", "", 1e-6));
    if (c.slope.stated()) out.push_back(rules::found("slope", "Stair: twice the riser plus the going", subject, c.slope, 2 * sr.riser + st.going, "m", "", 1e-6));
    if (c.flight.stated()) out.push_back(rules::found("flight", "Stair: risers in a flight", subject, c.flight, static_cast<double>(st.risers), "count"));
  }
  return out;
}

}  // namespace architecture::plan
