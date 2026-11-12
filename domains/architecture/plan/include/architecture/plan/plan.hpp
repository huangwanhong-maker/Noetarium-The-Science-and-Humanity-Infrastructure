// A building as it is drawn: walls on a plane, storey by storey, and what follows from them (ADR 0095).
//
// WHAT IS DRAWN. A storey is a plane graph: vertices (metres; x east, y north) and walls between them, each with a thickness, solid or
// open (an open wall is a boundary between two spaces with nothing built on it, as between a kitchen and a living room). On the walls
// stand openings, doors and windows, each at a distance along its wall. Labels are points, each naming the use of the room it lies in.
// Stairs join a storey to the one above. Nothing else is stored: the rooms are not drawn, they follow.
//
// WHAT FOLLOWS (read).
// - THE ROOMS are the bounded faces of the plane graph (physis's plane_faces: the orbits of the face permutation). A component of
//   walls standing free inside a room (a core, a pier, a wall that divides nothing) is a hole of that room: its outer walk lies in the
//   smallest room that contains it. A room's area is its face's, less its holes', measured to the walls' centrelines.
// - EACH WALL HAS TWO SIDES, the rooms on its left and on its right (or neither: the outside). A wall is an OUTER wall when one side
//   is an indoor room and the other is not (the outside, or a court or terrace); it divides nothing when both sides are the same.
// - THE NET AREA of a room is what is left inside its walls' faces: the points of the room at least half a wall's thickness from each
//   of its walls, physis's inset of the ring by t_k / 2 on edge k, exact while the mitred offset is. Where a room has holes or its
//   walk repeats a vertex, the first-order area A − Σ L_k t_k / 2 is given and said to be approximate.
// - THE WIDTH of a room is twice the largest d for which the uniform inset of its net ring by d is not empty, by bisection. For a
//   convex room it is the diameter of the largest circle the room holds, and for a rectangle its lesser side: the clear width a rule
//   of passage asks of a corridor. For a room with reflex corners the mitred inset lies within the true one, so the width is a bound
//   from below on that diameter (for an L, its wider arm).
// - LIGHT AND AIR. An opening on an outer wall lights and airs the indoor room on its inner side: its area times its glazed share,
//   and times its openable share.
// - THE ACCESS GRAPH. Its vertices are the outside and every room of every storey; a door or an open wall joins the two spaces
//   either side of its wall, a stair the room at its foot and the room at its head. A room's DEPTH is its distance from the outside
//   (morphe's shortest paths, unit weights); a room is reached THROUGH another when every way to it from the outside passes that
//   one (a dominator, found by removal); its RELATIVE ASYMMETRY is 2 (MD − 1) / (k − 2), MD its mean depth to the other k − 1 spaces
//   reached (Hillier and Hanson 1984, The Social Logic of Space, 108–109): 0 for the hub of a star, 1 for the end of a chain.
//
// WHAT IS REFUSED. A storey whose walls cross, or pass through a vertex without ending there, or repeat, is not a plane graph: its
// faults are named and no rooms are read. An opening beyond its wall's ends, two openings overlapping, a label outside every room
// or a second label in a room are remarked, and the rest is read.
//
// Left out: curved walls; rooms under a sloping ceiling; the void a stair needs in the floor above; stairs that turn.

#pragma once

#include <morphe/combinatorics/shortest_paths.hpp>
#include <physis/computational_geometry/planar.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace architecture::plan {

namespace cg = physis::computational_geometry;
using Point = cg::Point2;
using Ring = std::vector<Point>;
inline constexpr std::size_t none = static_cast<std::size_t>(-1);

inline double distance(Point a, Point b) { return std::hypot(b[0] - a[0], b[1] - a[1]); }

// ---- the uses of rooms ---------------------------------------------------------------------------------------------------------

struct Use {
  bool indoor = true;        // under the roof and within the outer walls
  bool habitable = false;    // a room people live or sleep in: it needs light and air
  bool wet = false;          // it has water and a drain
  bool circulation = false;  // a space one passes through
};

// The uses the domain knows; any other kind is an indoor room of no class.
inline Use use_of(std::string_view kind) {
  for (const char* k : {"living", "dining", "family", "bedroom", "study", "kitchen"})
    if (kind == k) return {true, true, kind == "kitchen", false};
  for (const char* k : {"bathroom", "ensuite", "toilet", "laundry"})
    if (kind == k) return {true, false, true, false};
  for (const char* k : {"hall", "corridor", "entry", "landing"})
    if (kind == k) return {true, false, false, true};
  for (const char* k : {"court", "terrace", "verandah", "balcony", "garden", "carport"})
    if (kind == k) return {false, false, false, false};
  return {};
}

inline const std::vector<std::string>& kinds() {
  static const std::vector<std::string> all{"living", "dining", "family", "kitchen", "bedroom", "study", "bathroom", "ensuite", "toilet", "laundry",
                                            "hall", "corridor", "entry", "landing", "store", "pantry", "wardrobe", "garage", "court", "terrace",
                                            "verandah", "balcony", "garden", "carport"};
  return all;
}

// ---- what is drawn -------------------------------------------------------------------------------------------------------------

enum class WallKind { solid, open };

struct Wall {
  std::size_t a = 0, b = 0;
  double thickness = 0.11;   // m
  WallKind kind = WallKind::solid;
};

enum class OpeningKind { door, window };

struct Opening {
  std::size_t wall = 0;
  OpeningKind kind = OpeningKind::door;
  double at = 0;        // the distance of its centre from the wall's vertex a (m)
  double width = 0.82;  // its clear width (m)
  double height = 2.04; // (m)
  double sill = 0;      // the height of its foot above the floor (m)
  double glazed = 0;    // the share of its area that is glass
  double openable = 1;  // the share of its area that opens
  int swing = 0;        // a door's leaf: +1 swings to its wall's left, −1 to its right; 0 into the room, or into the building
};

struct Label {
  Point at{};
  std::string kind, name;
};

struct Storey {
  std::string name;
  std::vector<Point> vertices;
  std::vector<Wall> walls;
  std::vector<Opening> openings;
  std::vector<Label> labels;
  double height = 3.0;    // floor to floor (m)
  double ceiling = 2.7;   // floor to ceiling (m)
};

struct Stair {
  std::size_t from = 0;   // it rises from this storey to the next
  Point at{};             // the middle of its first riser
  double bearing = 0;     // the direction it rises in, radians counter-clockwise from east
  double width = 1.0, going = 0.25;
  int risers = 17;
};

struct Building {
  std::vector<Storey> storeys;
  std::vector<Stair> stairs;
};

// ---- what follows from a storey ------------------------------------------------------------------------------------------------

struct Room {
  Ring ring;                                       // counter-clockwise, on the walls' centrelines
  std::vector<Ring> holes;                         // the outer walks of what stands free inside it
  std::vector<std::pair<std::size_t, int>> walls;  // its boundary walk, holes' after its own: (wall, +1 walked from a to b)
  bool simple = true;                              // its own walk visits no vertex twice
  double area = 0, net_area = 0, perimeter = 0;
  bool net_exact = true;
  Ring net_ring;                                   // the ring inside the walls' faces, where the inset is exact
  double width = std::numeric_limits<double>::quiet_NaN();
  std::size_t label = none;
  std::string kind, name;
  double outer_length = 0, glazed_area = 0, openable_area = 0;
};

struct Side {
  std::size_t left = none, right = none;
  double length = 0;
  bool outer = false, divides_nothing = false;
};

struct StoreyReading {
  bool sound = true;
  std::vector<std::string> faults, remarks;
  std::vector<std::size_t> faulty_walls;
  std::vector<Room> rooms;
  std::vector<Side> sides;
  double floor_area = 0;      // the indoor rooms, to the walls' centrelines
  double internal_area = 0;   // the indoor rooms, inside the walls' faces
};

namespace detail {

// Whether p lies on the segment a–b away from its ends.
inline bool on_interior(Point p, Point a, Point b, double eps) {
  const double L = distance(a, b);
  if (L <= eps) return false;
  const double t = ((p[0] - a[0]) * (b[0] - a[0]) + (p[1] - a[1]) * (b[1] - a[1])) / (L * L);
  if (t * L <= eps || (1 - t) * L <= eps) return false;
  return cg::segment_distance(p, a, b) <= eps;
}

// A ring without the vertices at which two edges of one offset run straight on: the mitred offset meets no parallel lines there.
inline void straighten(Ring& ring, std::vector<double>& d) {
  bool changed = true;
  while (changed && ring.size() > 3) {
    changed = false;
    const std::size_t n = ring.size();
    for (std::size_t i = 0; i < n; i++) {
      const std::size_t h = (i + n - 1) % n;
      const Point a = ring[h], b = ring[i], c = ring[(i + 1) % n];
      const double ux = b[0] - a[0], uy = b[1] - a[1], vx = c[0] - b[0], vy = c[1] - b[1];
      const double cross = ux * vy - uy * vx, dot = ux * vx + uy * vy;
      if (std::fabs(cross) <= 1e-9 * std::hypot(ux, uy) * std::hypot(vx, vy) && dot > 0 && std::fabs(d[h] - d[i]) < 1e-12) {
        ring.erase(ring.begin() + static_cast<std::ptrdiff_t>(i));
        d.erase(d.begin() + static_cast<std::ptrdiff_t>(i));
        changed = true;
        break;
      }
    }
  }
}

}  // namespace detail

// Twice the largest uniform inset a ring admits, by bisection on physis's inset (to 2⁻⁴⁰ of the bound √(A/π)): the diameter of the
// largest circle a convex ring holds, and a bound from below on it otherwise.
inline double inscribed_diameter(const Ring& ring) {
  if (ring.size() < 3) return 0;
  double lo = 0, hi = std::sqrt(std::fabs(cg::ring_area(ring)) / std::numbers::pi);
  for (int it = 0; it < 40; it++) {
    const double mid = (lo + hi) / 2;
    if (!cg::inset(ring, std::vector<double>(ring.size(), mid)).empty()) lo = mid;
    else hi = mid;
  }
  return 2 * lo;
}

// The room a point lies in: the smallest whose ring contains it; none outside every room.
inline std::size_t locate(const StoreyReading& R, Point p) {
  std::size_t host = none;
  double best = std::numeric_limits<double>::infinity();
  for (std::size_t r = 0; r < R.rooms.size(); r++) {
    const double a = std::fabs(cg::ring_area(R.rooms[r].ring));
    if (a < best && cg::inside_ring(R.rooms[r].ring, p)) best = a, host = r;
  }
  return host;
}

inline StoreyReading read(const Storey& s, double eps = 1e-6) {
  StoreyReading R;
  const auto& V = s.vertices;
  const std::size_t E = s.walls.size();
  R.sides.assign(E, Side{});
  const auto fault = [&](std::string what, std::initializer_list<std::size_t> walls) {
    R.faults.push_back(std::move(what)), R.sound = false;
    for (const std::size_t w : walls)
      if (std::find(R.faulty_walls.begin(), R.faulty_walls.end(), w) == R.faulty_walls.end()) R.faulty_walls.push_back(w);
  };
  const auto nth = [](std::size_t k) { return std::to_string(k + 1); };

  // Is it a plane graph?
  std::vector<char> used(V.size(), 0);
  for (std::size_t e = 0; e < E; e++) {
    const auto& w = s.walls[e];
    if (w.a >= V.size() || w.b >= V.size()) { fault("wall " + nth(e) + " ends at no vertex", {e}); continue; }
    R.sides[e].length = distance(V[w.a], V[w.b]);
    if (R.sides[e].length <= eps) fault("wall " + nth(e) + " has no length", {e});
    used[w.a] = used[w.b] = 1;
  }
  if (!R.sound) return R;
  for (std::size_t i = 0; i < V.size(); i++)
    for (std::size_t j = i + 1; j < V.size(); j++)
      if (used[i] && used[j] && distance(V[i], V[j]) <= eps) fault("two vertices coincide: " + nth(i) + " and " + nth(j), {});
  for (std::size_t i = 0; i < E; i++)
    for (std::size_t j = i + 1; j < E; j++) {
      const auto& p = s.walls[i];
      const auto& q = s.walls[j];
      if ((p.a == q.a && p.b == q.b) || (p.a == q.b && p.b == q.a)) fault("walls " + nth(i) + " and " + nth(j) + " are the same wall", {i, j});
      else if (cg::segments_cross(V[p.a], V[p.b], V[q.a], V[q.b])) fault("walls " + nth(i) + " and " + nth(j) + " cross", {i, j});
    }
  for (std::size_t e = 0; e < E; e++)
    for (std::size_t v = 0; v < V.size(); v++)
      if (used[v] && v != s.walls[e].a && v != s.walls[e].b && detail::on_interior(V[v], V[s.walls[e].a], V[s.walls[e].b], eps))
        fault("wall " + nth(e) + " passes through a vertex without ending there", {e});
  if (!R.sound) return R;

  // The faces, and the components of the walls.
  std::vector<cg::PlaneEdge> edges(E);
  for (std::size_t e = 0; e < E; e++) edges[e].u = s.walls[e].a, edges[e].v = s.walls[e].b;
  const auto faces = cg::plane_faces(V, edges);
  std::vector<std::size_t> parent(V.size());
  std::iota(parent.begin(), parent.end(), std::size_t{0});
  const auto find = [&](std::size_t v) {
    while (parent[v] != v) v = parent[v] = parent[parent[v]];
    return v;
  };
  for (const auto& w : s.walls) parent[find(w.a)] = find(w.b);

  std::vector<std::size_t> outers, component;
  std::vector<double> outer_area;
  for (std::size_t f = 0; f < faces.size(); f++) {
    const auto& face = faces[f];
    if (!(face.area > 1e-9)) { outers.push_back(f); continue; }
    Room r;
    for (const std::size_t v : face.vertices) r.ring.push_back(V[v]);
    r.walls = face.edges, r.simple = face.simple, r.area = face.area;
    for (const auto& [e, dir] : face.edges) r.perimeter += R.sides[e].length;
    component.push_back(find(face.vertices.front()));
    outer_area.push_back(face.area);
    R.rooms.push_back(std::move(r));
  }
  for (std::size_t r = 0; r < R.rooms.size(); r++)
    for (const auto& [e, dir] : R.rooms[r].walls) (dir > 0 ? R.sides[e].left : R.sides[e].right) = r;
  // What stands free inside a room is a hole of it.
  for (const std::size_t f : outers) {
    const auto& face = faces[f];
    if (face.vertices.empty()) continue;
    const Point p = V[face.vertices.front()];
    const std::size_t c = find(face.vertices.front());
    std::size_t host = none;
    double best = std::numeric_limits<double>::infinity();
    for (std::size_t r = 0; r < R.rooms.size(); r++)
      if (component[r] != c && outer_area[r] < best && cg::inside_ring(R.rooms[r].ring, p)) best = outer_area[r], host = r;
    if (host == none) continue;
    Ring hole;
    for (const std::size_t v : face.vertices) hole.push_back(V[v]);
    auto& room = R.rooms[host];
    room.holes.push_back(std::move(hole));
    room.area -= std::fabs(face.area);
    for (const auto& [e, dir] : face.edges) {
      room.perimeter += R.sides[e].length;
      (dir > 0 ? R.sides[e].left : R.sides[e].right) = host;
      room.walls.push_back({e, dir});
    }
  }

  // The labels name the rooms they lie in.
  for (std::size_t i = 0; i < s.labels.size(); i++) {
    const std::size_t host = locate(R, s.labels[i].at);
    if (host == none) { R.remarks.push_back("label " + nth(i) + " lies in no room"); continue; }
    if (R.rooms[host].label != none) { R.remarks.push_back("label " + nth(i) + " is a second label in its room"); continue; }
    R.rooms[host].label = i, R.rooms[host].kind = s.labels[i].kind, R.rooms[host].name = s.labels[i].name;
  }
  const auto indoor = [&](std::size_t r) { return r != none && use_of(R.rooms[r].kind).indoor; };
  for (std::size_t e = 0; e < E; e++) {
    auto& side = R.sides[e];
    side.divides_nothing = side.left == side.right;
    side.outer = indoor(side.left) != indoor(side.right);
  }

  // Net areas and widths.
  for (auto& room : R.rooms) {
    double first_order = room.area;
    for (const auto& [e, dir] : room.walls)
      if (s.walls[e].kind == WallKind::solid) first_order -= R.sides[e].length * s.walls[e].thickness / 2;
    first_order = std::max(0.0, first_order);
    room.net_area = first_order, room.net_exact = false;
    if (room.simple && room.holes.empty()) {
      Ring ring = room.ring;
      std::vector<double> d(ring.size());
      for (std::size_t k = 0; k < ring.size(); k++) d[k] = s.walls[room.walls[k].first].kind == WallKind::solid ? s.walls[room.walls[k].first].thickness / 2 : 0.0;
      detail::straighten(ring, d);
      const Ring in = cg::inset(ring, d);
      const double a = in.empty() ? 0.0 : cg::ring_area(in);
      // The clipped fallback of the inset may lose an arm of a room with reflex corners: then the first order is kept.
      if (!in.empty() && a >= first_order - 0.02 * room.area - 0.05) {
        room.net_area = a, room.net_exact = true, room.net_ring = in;
        room.width = inscribed_diameter(in);
      }
    }
  }

  // Openings: where they stand, and what they light and air.
  std::vector<std::vector<std::pair<double, double>>> spans(E);
  for (std::size_t i = 0; i < s.openings.size(); i++) {
    const auto& o = s.openings[i];
    if (o.wall >= E) { R.remarks.push_back("opening " + nth(i) + " stands on no wall"); continue; }
    const double L = R.sides[o.wall].length;
    if (!(o.width > 0) || o.at - o.width / 2 < -eps || o.at + o.width / 2 > L + eps) {
      R.remarks.push_back("opening " + nth(i) + " runs beyond the ends of wall " + nth(o.wall));
      continue;
    }
    for (const auto& [lo, hi] : spans[o.wall])
      if (o.at - o.width / 2 < hi - eps && o.at + o.width / 2 > lo + eps) R.remarks.push_back("opening " + nth(i) + " overlaps another on wall " + nth(o.wall));
    spans[o.wall].push_back({o.at - o.width / 2, o.at + o.width / 2});
    const auto& side = R.sides[o.wall];
    if (!side.outer) {
      if (o.kind == OpeningKind::window) R.remarks.push_back("window " + nth(i) + " stands between two rooms: it lights neither");
      continue;
    }
    const std::size_t inner = indoor(side.left) ? side.left : side.right;
    R.rooms[inner].glazed_area += o.width * o.height * o.glazed;
    R.rooms[inner].openable_area += o.width * o.height * o.openable;
  }
  for (std::size_t r = 0; r < R.rooms.size(); r++) {
    auto& room = R.rooms[r];
    for (const auto& [e, dir] : room.walls)
      if (R.sides[e].outer) room.outer_length += R.sides[e].length;
    if (indoor(r)) R.floor_area += room.area, R.internal_area += room.net_area;
  }
  return R;
}

// ---- what follows from the building --------------------------------------------------------------------------------------------

struct StairReading {
  std::size_t foot = none, head = none;   // the spaces at its ends, as vertices of the access graph
  double riser = 0, run = 0;
  Ring ring;                              // its plan
};

struct Reading {
  std::vector<StoreyReading> storeys;
  std::vector<std::size_t> first;                           // the access graph's vertex of each storey's room 0
  std::vector<std::pair<std::size_t, std::size_t>> spaces;  // vertex n ≥ 1 is spaces[n − 1] = (storey, room); vertex 0 is the outside
  std::vector<std::vector<std::size_t>> access;
  std::vector<double> depth;                                // from the outside; +∞ where not reached
  std::vector<std::vector<std::size_t>> through;            // the spaces every way to it passes
  std::vector<double> asymmetry;                            // relative asymmetry among the spaces reached; NaN where undefined
  std::vector<StairReading> stairs;
  std::vector<std::string> remarks;
  bool sound = true;
  std::size_t vertex(std::size_t storey, std::size_t room) const { return first[storey] + room; }
};

inline Reading read(const Building& b) {
  Reading R;
  std::size_t next = 1;
  for (const auto& s : b.storeys) {
    R.storeys.push_back(read(s));
    R.sound = R.sound && R.storeys.back().sound;
    R.first.push_back(next);
    for (std::size_t r = 0; r < R.storeys.back().rooms.size(); r++) R.spaces.push_back({R.storeys.size() - 1, r});
    next += R.storeys.back().rooms.size();
  }
  const std::size_t N = next;
  R.access.assign(N, {});
  const auto join = [&](std::size_t u, std::size_t v) {
    if (u == v) return;
    if (std::find(R.access[u].begin(), R.access[u].end(), v) == R.access[u].end()) R.access[u].push_back(v), R.access[v].push_back(u);
  };
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    if (!S.sound) continue;
    const auto vertex = [&](std::size_t room) { return room == none ? std::size_t{0} : R.first[k] + room; };
    for (std::size_t e = 0; e < s.walls.size(); e++)
      if (s.walls[e].kind == WallKind::open) join(vertex(S.sides[e].left), vertex(S.sides[e].right));
    for (const auto& o : s.openings)
      if (o.kind == OpeningKind::door && o.wall < s.walls.size()) join(vertex(S.sides[o.wall].left), vertex(S.sides[o.wall].right));
  }
  for (std::size_t i = 0; i < b.stairs.size(); i++) {
    const auto& st = b.stairs[i];
    StairReading sr;
    const double ux = std::cos(st.bearing), uy = std::sin(st.bearing);
    sr.run = std::max(0, st.risers - 1) * st.going;
    const Point head{st.at[0] + sr.run * ux, st.at[1] + sr.run * uy};
    const double nx = -uy * st.width / 2, ny = ux * st.width / 2;
    sr.ring = {{st.at[0] - nx, st.at[1] - ny}, {head[0] - nx, head[1] - ny}, {head[0] + nx, head[1] + ny}, {st.at[0] + nx, st.at[1] + ny}};
    if (st.from + 1 >= b.storeys.size()) {
      R.remarks.push_back("stair " + std::to_string(i + 1) + " rises to no storey");
    } else if (R.storeys[st.from].sound && R.storeys[st.from + 1].sound) {
      sr.riser = st.risers > 0 ? b.storeys[st.from].height / st.risers : 0.0;
      const std::size_t foot = locate(R.storeys[st.from], st.at), top = locate(R.storeys[st.from + 1], head);
      if (foot == none) R.remarks.push_back("the foot of stair " + std::to_string(i + 1) + " lies in no room");
      if (top == none) R.remarks.push_back("the head of stair " + std::to_string(i + 1) + " lies in no room of the storey above");
      if (foot != none && top != none) {
        sr.foot = R.vertex(st.from, foot), sr.head = R.vertex(st.from + 1, top);
        join(sr.foot, sr.head);
      }
    }
    R.stairs.push_back(std::move(sr));
  }

  // Depths from the outside, the spaces each is reached through, and relative asymmetry.
  namespace mc = morphe::combinatorics;
  const auto graph_without = [&](std::size_t gone) {
    mc::WeightedAdjacency g(N);
    for (std::size_t u = 0; u < N; u++)
      for (const std::size_t v : R.access[u])
        if (u != gone && v != gone) g[u].push_back({v, 1.0});
    return g;
  };
  const auto whole = graph_without(none);
  R.depth = mc::dijkstra(whole, 0).distance;
  R.through.assign(N, {});
  for (std::size_t y = 1; y < N; y++) {
    if (!std::isfinite(R.depth[y]) || R.access[y].size() < 2) continue;
    const auto d = mc::dijkstra(graph_without(y), 0).distance;
    for (std::size_t x = 1; x < N; x++)
      if (x != y && std::isfinite(R.depth[x]) && !std::isfinite(d[x])) R.through[x].push_back(y);
  }
  std::size_t reached = 0;
  for (std::size_t v = 0; v < N; v++) reached += std::isfinite(R.depth[v]);
  R.asymmetry.assign(N, std::numeric_limits<double>::quiet_NaN());
  if (reached > 2)
    for (std::size_t v = 0; v < N; v++) {
      if (!std::isfinite(R.depth[v])) continue;
      const auto d = mc::dijkstra(whole, v).distance;
      double total = 0;
      for (std::size_t u = 0; u < N; u++)
        if (std::isfinite(d[u])) total += d[u];
      const double mean = total / static_cast<double>(reached - 1);
      R.asymmetry[v] = 2 * (mean - 1) / static_cast<double>(reached - 2);
    }
  return R;
}

// A room's name as a reader meets it: its own, or its kind, or "a room".
inline std::string called(const Room& r) {
  if (!r.name.empty()) return r.name;
  if (!r.kind.empty()) return r.kind;
  return "a room";
}

}  // namespace architecture::plan
