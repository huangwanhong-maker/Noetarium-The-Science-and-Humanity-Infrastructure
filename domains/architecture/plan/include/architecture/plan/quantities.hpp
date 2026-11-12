// What a drawn building is made of, what it would cost at declared rates, and the heat its fabric loses (ADR 0095).
//
// QUANTITIES, taken from the drawing:
// - floor: each storey's indoor rooms to the walls' centrelines; the ground slab is the first storey's, the upper floors the others';
// - outer walls: their length times the storey's height, gross, and net of the openings that stand on them; by the side they face
//   (north, east, south, west: the quadrant of the outward normal's bearing), with the glass on each side;
// - inner walls: the solid walls between two indoor rooms or standing within one, their length times the ceiling's height, net of
//   their doors;
// - windows by area, doors to the outside and between rooms by number, stairs by number;
// - the roof: what each storey covers that the storey above does not, Σ_k max(0, A_k − A_{k+1}), exact when the storeys nest, times
//   a factor for its pitch (1 for a flat roof, 1 / cos of the pitch otherwise);
// - wet rooms' net floor.
// Walls with the open air on both sides (a garden wall) are not counted.
//
// COST. Each quantity times a rate, and a rate is an interval [low, high] that the person declares: the cost is the interval
// [Σ q low, Σ q high], exact for quantities and rates that are not negative. No rate here comes from a source.
//
// HEAT. The fabric's transmission coefficient H = Σ U A over the outer walls (net), the windows, the outer doors, the roof (in plan)
// and the ground slab, in W/K: the steady loss per kelvin between inside and outside. Thermal bridges, the air exchanged, and the
// ground's own resistance are left out; it compares designs, and is not a prediction of a bill.

#pragma once

#include <architecture/plan/plan.hpp>

#include <array>
#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace architecture::plan {

struct Quantities {
  double floor_area = 0, internal_area = 0, wet_area = 0;
  double ground_slab = 0, upper_floor = 0, roof_plan = 0, roof = 0;
  double outer_length = 0, outer_wall = 0, outer_wall_net = 0, inner_wall_net = 0;
  double window_area = 0, glass_area = 0, outer_door_area = 0;
  int windows = 0, outer_doors = 0, inner_doors = 0, stairs = 0;
  std::array<double, 4> wall_by_side{}, glass_by_side{};   // north, east, south, west
};

// The side an outward normal faces: 0 north, 1 east, 2 south, 3 west (x east, y north).
inline int side_of(double nx, double ny) {
  const double bearing = std::atan2(nx, ny) * 180 / std::numbers::pi;   // from north, clockwise
  if (bearing >= -45 && bearing < 45) return 0;
  if (bearing >= 45 && bearing < 135) return 1;
  if (bearing >= -135 && bearing < -45) return 3;
  return 2;
}

inline Quantities quantities(const Building& b, const Reading& R, double roof_factor = 1.0) {
  Quantities q;
  std::vector<double> floors;
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    floors.push_back(S.sound ? S.floor_area : 0.0);
    if (!S.sound) continue;
    q.floor_area += S.floor_area, q.internal_area += S.internal_area;
    (k == 0 ? q.ground_slab : q.upper_floor) += S.floor_area;
    for (const auto& room : S.rooms)
      if (use_of(room.kind).indoor && use_of(room.kind).wet) q.wet_area += room.net_area;
    const auto indoor = [&](std::size_t r) { return r != none && use_of(S.rooms[r].kind).indoor; };
    std::vector<double> holes(s.walls.size(), 0.0);
    for (const auto& o : s.openings) {
      if (o.wall >= s.walls.size()) continue;
      const auto& side = S.sides[o.wall];
      const double a = o.width * o.height;
      holes[o.wall] += a;
      if (side.outer) {
        if (o.kind == OpeningKind::window) q.windows++, q.window_area += a;
        else q.outer_doors++, q.outer_door_area += a;
        q.glass_area += a * o.glazed;
      } else if (o.kind == OpeningKind::door && indoor(side.left) && indoor(side.right)) {
        q.inner_doors++;
      }
    }
    for (std::size_t e = 0; e < s.walls.size(); e++) {
      const auto& w = s.walls[e];
      const auto& side = S.sides[e];
      if (w.kind != WallKind::solid) continue;
      if (side.outer) {
        const double gross = side.length * s.height;
        q.outer_length += side.length, q.outer_wall += gross, q.outer_wall_net += std::max(0.0, gross - holes[e]);
        // The outward normal: the left of a → b is (−dy, dx); it points out when the indoor room is on the right.
        const double dx = s.vertices[w.b][0] - s.vertices[w.a][0], dy = s.vertices[w.b][1] - s.vertices[w.a][1];
        const bool inside_left = indoor(side.left);
        const int face = side_of(inside_left ? dy : -dy, inside_left ? -dx : dx);
        q.wall_by_side[face] += gross;
        for (const auto& o : s.openings)
          if (o.wall == e) q.glass_by_side[face] += o.width * o.height * o.glazed;
      } else if (indoor(side.left) && indoor(side.right)) {
        q.inner_wall_net += std::max(0.0, side.length * s.ceiling - holes[e]);
      }
    }
  }
  for (std::size_t k = 0; k < floors.size(); k++) q.roof_plan += std::max(0.0, floors[k] - (k + 1 < floors.size() ? floors[k + 1] : 0.0));
  q.roof = q.roof_plan * roof_factor;
  q.stairs = static_cast<int>(b.stairs.size());
  return q;
}

// ---- cost ----------------------------------------------------------------------------------------------------------------------

struct Rate {
  double low = 0, high = 0;
};

struct CostItem {
  std::string id, unit;
  double quantity = 0, low = 0, high = 0;
};

struct Cost {
  std::vector<CostItem> items;
  double low = 0, high = 0;
};

// The elements a rate may be declared for, with the quantity each multiplies.
inline std::vector<CostItem> elements(const Quantities& q) {
  return {{"outer_wall", "m²", q.outer_wall_net},   {"inner_wall", "m²", q.inner_wall_net}, {"window", "m²", q.window_area},
          {"outer_door", "each", double(q.outer_doors)}, {"inner_door", "each", double(q.inner_doors)}, {"ground_slab", "m²", q.ground_slab},
          {"upper_floor", "m²", q.upper_floor},     {"roof", "m²", q.roof},                 {"stair", "each", double(q.stairs)},
          {"fit_out", "m²", q.internal_area},       {"wet_room", "m²", q.wet_area}};
}

inline Cost cost(const Quantities& q, const std::map<std::string, Rate>& rates) {
  Cost c;
  for (auto item : elements(q)) {
    const auto r = rates.find(item.id);
    if (r == rates.end()) continue;
    item.low = item.quantity * r->second.low, item.high = item.quantity * r->second.high;
    c.low += item.low, c.high += item.high;
    c.items.push_back(std::move(item));
  }
  return c;
}

// ---- heat ----------------------------------------------------------------------------------------------------------------------

struct Fabric {
  double wall = 0.5, window = 2.8, door = 2.5, roof = 0.25, ground = 0.6;   // U-values, W/(m² K): declared, not any code's
};

struct HeatLoss {
  double wall = 0, window = 0, door = 0, roof = 0, ground = 0;   // each element's U A (W/K)
  double total = 0, per_floor = 0;                               // H, and H per m² of floor
};

inline HeatLoss heat_loss(const Quantities& q, const Fabric& f) {
  HeatLoss h;
  h.wall = f.wall * q.outer_wall_net, h.window = f.window * q.window_area, h.door = f.door * q.outer_door_area;
  h.roof = f.roof * q.roof_plan, h.ground = f.ground * q.ground_slab;
  h.total = h.wall + h.window + h.door + h.roof + h.ground;
  h.per_floor = q.floor_area > 0 ? h.total / q.floor_area : 0.0;
  return h;
}

}  // namespace architecture::plan
