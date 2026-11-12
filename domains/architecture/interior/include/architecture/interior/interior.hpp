// What stands in a building's rooms, and whether it can be used (ADR 0096): furniture and fixtures as pieces on the plan, each with
// the space it needs before it, and what follows of a room that holds them.
//
// A PIECE is a rectangle on the floor: a place (the middle of its footprint), a bearing (the direction its front faces), a width
// across its front, a depth from front to back and a height. It may need space to be used: a depth of clear floor in FRONT of it
// (the legroom of a sofa, the standing room at a counter, the opening of a wardrobe's doors), at its SIDES (the two sides of a
// double bed), or all ROUND it (a table whose chairs are drawn back, an island). A piece OBSTRUCTS when it stands on the floor and cannot be walked through; a rug, a picture and a lamp hung from
// the ceiling do not. Its local frame: lx to its right along its width, ly toward its front; the footprint is |lx| ≤ w/2, |ly| ≤ d/2.
//
// WHAT IS FOUND OF EACH PIECE (read), in the room its middle lies in:
// - WITHIN: its footprint lies in the room, inside the walls' faces (the room's net ring; plan.hpp), crossing no wall;
// - APART: it overlaps no other piece that obstructs. Two convex footprints overlap when one clipped by the other's edges
//   (physis's clip_left, Sutherland and Hodgman 1974) keeps an area;
// - USE: the space it needs lies in the room and no piece that obstructs stands in it. Two pieces' spaces may overlap: one stands
//   in front of a sofa and of a table in turn;
// - REACHED: a person can come from a door of the room to the space it needs. A person is a disc of the diameter `passage`. The
//   room's free floor (its net ring, less the pieces that obstruct and the stairs) is sampled on a grid of `cell` metres; where the
//   disc's middle can stand is that set eroded by the disc's radius (eikon's exact distance transform); a piece is reached when
//   its space, grown by the radius, holds a point of the component (eikon's components, edge-connected) that a door of the room
//   opens onto. Exact to about one cell: a gap within one cell of the passage may be found either way.
// And of each door: that the square its leaf sweeps holds no piece that obstructs, and that the disc can stand behind it.
//
// A door swings into the room on its wall's left, or into the building where its wall is an outer wall, unless it is drawn to swing
// the other way (plan.hpp's Opening::swing).
//
// The depths of the spaces are a catalogue's or a person's to state; none here is a standard's. Left out: pieces that are not
// rectangles; a piece under another (a stool under a counter); the turning circle of a wheelchair; what a piece is connected to.

#pragma once

#include <architecture/plan/plan.hpp>
#include <eikon/distance/distance.hpp>
#include <eikon/segmentation/segmentation.hpp>
#include <physis/computational_geometry/planar.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace architecture::interior {

namespace ap = architecture::plan;
namespace cg = physis::computational_geometry;
using ap::none;
using ap::Point;
using ap::Ring;

struct Piece {
  std::string kind, name;
  std::size_t storey = 0;
  Point at{};
  double bearing = 0;                 // radians, counter-clockwise from east: the direction its front faces
  double width = 1, depth = 1, height = 1;
  double front = 0, side = 0;         // the clear floor it needs before it and at each side (m)
  double around = 0;                  // or all round it, in place of those
  bool obstructs = true;
};

// The piece's right and its front, as unit vectors on the ground.
inline std::array<Point, 2> axes(const Piece& p) {
  const double c = std::cos(p.bearing), s = std::sin(p.bearing);
  return {Point{s, -c}, Point{c, s}};
}

inline Point place(const Piece& p, double lx, double ly) {
  const auto [u, f] = axes(p);
  return {p.at[0] + lx * u[0] + ly * f[0], p.at[1] + lx * u[1] + ly * f[1]};
}

// A rectangle of the piece's local frame, counter-clockwise on the ground.
inline Ring rectangle(const Piece& p, double x0, double y0, double x1, double y1) { return {place(p, x0, y0), place(p, x1, y0), place(p, x1, y1), place(p, x0, y1)}; }

inline Ring footprint(const Piece& p) { return rectangle(p, -p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2); }

// The spaces it needs: before it, and at each side.
inline std::vector<Ring> spaces(const Piece& p) {
  std::vector<Ring> out;
  const double w = p.width / 2, d = p.depth / 2, a = p.around;
  if (a > 0) return {rectangle(p, -w - a, d, w + a, d + a), rectangle(p, -w - a, -d - a, w + a, -d), rectangle(p, w, -d, w + a, d), rectangle(p, -w - a, -d, -w, d)};
  if (p.front > 0) out.push_back(rectangle(p, -p.width / 2, p.depth / 2, p.width / 2, p.depth / 2 + p.front));
  if (p.side > 0) {
    out.push_back(rectangle(p, p.width / 2, -p.depth / 2, p.width / 2 + p.side, p.depth / 2));
    out.push_back(rectangle(p, -p.width / 2 - p.side, -p.depth / 2, -p.width / 2, p.depth / 2));
  }
  return out;
}

// The area two convex rings share (both counter-clockwise).
inline double shared_area(const Ring& a, const Ring& b) {
  Ring r = a;
  for (std::size_t k = 0; k < b.size() && r.size() >= 3; k++) r = cg::clip_left(r, b[k], b[(k + 1) % b.size()]);
  return r.size() >= 3 ? std::fabs(cg::ring_area(r)) : 0.0;
}

namespace detail {

// A ring drawn in toward its middle by a small length, so that a piece flush against a wall counts as within it.
inline Ring shrunk(const Ring& r, double by) {
  Point c{0, 0};
  for (const auto& p : r) c[0] += p[0] / static_cast<double>(r.size()), c[1] += p[1] / static_cast<double>(r.size());
  Ring out;
  for (const auto& p : r) {
    const double L = std::hypot(p[0] - c[0], p[1] - c[1]);
    out.push_back(L > by ? Point{p[0] + (c[0] - p[0]) * by / L, p[1] + (c[1] - p[1]) * by / L} : c);
  }
  return out;
}

// Whether a convex ring lies in a room: each corner inside its boundary and outside its holes, and no edge crossing either.
inline bool lies_in(const Ring& ring, const Ring& boundary, const std::vector<Ring>& holes) {
  const Ring r = shrunk(ring, 0.003);
  for (const auto& p : r) {
    if (!cg::inside_ring(boundary, p)) return false;
    for (const auto& h : holes)
      if (cg::inside_ring(h, p)) return false;
  }
  const auto crosses = [&](const Ring& other) {
    for (std::size_t i = 0; i < r.size(); i++)
      for (std::size_t j = 0; j < other.size(); j++)
        if (cg::segments_cross(r[i], r[(i + 1) % r.size()], other[j], other[(j + 1) % other.size()])) return true;
    return false;
  };
  if (crosses(boundary)) return false;
  for (const auto& h : holes)
    if (crosses(h)) return false;
  return true;
}

inline bool in_rectangle(const Piece& p, Point q, double x0, double y0, double x1, double y1) {
  const auto [u, f] = axes(p);
  const double dx = q[0] - p.at[0], dy = q[1] - p.at[1], lx = dx * u[0] + dy * u[1], ly = dx * f[0] + dy * f[1];
  return lx >= x0 && lx <= x1 && ly >= y0 && ly <= y1;
}

}  // namespace detail

struct PieceReading {
  std::size_t room = none;                 // of its storey; none when its middle lies in no room
  bool within = true;
  std::vector<std::size_t> overlaps;       // the pieces it overlaps
  bool space_within = true;
  std::vector<std::size_t> blocked_by;     // the pieces that stand in the space it needs
  bool reached = true;
  bool judged = false;                     // whether reaching it could be judged: its room has a door or an open wall
  bool ok() const { return room != none && within && overlaps.empty() && space_within && blocked_by.empty() && reached; }
};

struct DoorReading {
  std::size_t storey = 0, opening = 0;
  std::vector<std::size_t> swing_blocked_by;   // the pieces in the square its leaf sweeps
  bool entered = true;                         // a person can stand behind it, on each side that is a room
};

struct RoomFloor {
  std::size_t storey = 0, room = 0;
  double free_share = 1;                       // of its floor inside the walls, what no piece stands on
};

struct Reading {
  double passage = 0.6, cell = 0.05;
  std::vector<PieceReading> pieces;
  std::vector<DoorReading> doors;
  std::vector<RoomFloor> rooms;
};

// The side a door's leaf swings to: +1 the wall's left, −1 its right.
inline int swing_side(const ap::Opening& o, const ap::Side& side, const ap::StoreyReading& S) {
  if (o.swing != 0) return o.swing > 0 ? 1 : -1;
  const auto indoor = [&](std::size_t r) { return r != none && ap::use_of(S.rooms[r].kind).indoor; };
  if (side.outer) return indoor(side.left) ? 1 : -1;
  return 1;
}

inline Reading read(const ap::Building& b, const ap::Reading& R, const std::vector<Piece>& pieces, double passage = 0.6, double cell = 0.05) {
  Reading out;
  out.passage = passage, out.cell = cell;
  out.pieces.assign(pieces.size(), {});
  const double radius = passage / 2;

  // Each piece in its room: within it, apart from the others, its space free.
  std::vector<Ring> feet(pieces.size());
  for (std::size_t i = 0; i < pieces.size(); i++) feet[i] = footprint(pieces[i]);
  const auto boundary = [&](const ap::Room& room) -> const Ring& { return room.net_exact && room.net_ring.size() >= 3 ? room.net_ring : room.ring; };
  for (std::size_t i = 0; i < pieces.size(); i++) {
    const auto& p = pieces[i];
    auto& pr = out.pieces[i];
    if (p.storey >= R.storeys.size() || !R.storeys[p.storey].sound) continue;
    const auto& S = R.storeys[p.storey];
    pr.room = ap::locate(S, p.at);
    if (pr.room == none) { pr.within = false; continue; }
    const auto& room = S.rooms[pr.room];
    pr.within = detail::lies_in(feet[i], boundary(room), room.holes);
    for (const auto& space : spaces(p))
      if (!detail::lies_in(space, boundary(room), room.holes)) pr.space_within = false;
    for (std::size_t j = 0; j < pieces.size(); j++) {
      if (j == i || pieces[j].storey != p.storey || !pieces[j].obstructs) continue;
      if (p.obstructs && shared_area(feet[i], feet[j]) > 1e-4) pr.overlaps.push_back(j);
      for (const auto& space : spaces(p))
        if (shared_area(space, feet[j]) > 1e-4 && std::find(pr.blocked_by.begin(), pr.blocked_by.end(), j) == pr.blocked_by.end()) pr.blocked_by.push_back(j);
    }
  }

  // The doors' swings.
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    if (!S.sound) continue;
    for (std::size_t i = 0; i < s.openings.size(); i++) {
      const auto& o = s.openings[i];
      if (o.kind != ap::OpeningKind::door || o.wall >= s.walls.size()) continue;
      const auto& w = s.walls[o.wall];
      const Point A = s.vertices[w.a], B = s.vertices[w.b];
      const double L = ap::distance(A, B);
      if (!(L > 0)) continue;
      const Point d{(B[0] - A[0]) / L, (B[1] - A[1]) / L};
      const int side = swing_side(o, S.sides[o.wall], S);
      const Point n{-d[1] * side, d[0] * side};
      const double t = w.thickness / 2, a0 = o.at - o.width / 2, a1 = o.at + o.width / 2;
      const auto at = [&](double along, double off) { return Point{A[0] + d[0] * along + n[0] * off, A[1] + d[1] * along + n[1] * off}; };
      Ring square = {at(a0, t), at(a1, t), at(a1, t + o.width), at(a0, t + o.width)};
      if (cg::ring_area(square) < 0) std::reverse(square.begin(), square.end());
      DoorReading dr;
      dr.storey = k, dr.opening = i;
      for (std::size_t j = 0; j < pieces.size(); j++)
        if (pieces[j].storey == k && pieces[j].obstructs && shared_area(square, feet[j]) > 1e-4) dr.swing_blocked_by.push_back(j);
      out.doors.push_back(std::move(dr));
    }
  }

  // Reaching: room by room, on a grid.
  namespace eg = eikon::grid;
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    if (!S.sound) continue;
    for (std::size_t r = 0; r < S.rooms.size(); r++) {
      const auto& room = S.rooms[r];
      const Ring& ring = boundary(room);
      double x0 = std::numeric_limits<double>::infinity(), y0 = x0, x1 = -x0, y1 = -x0;
      for (const auto& p : ring) x0 = std::min(x0, p[0]), y0 = std::min(y0, p[1]), x1 = std::max(x1, p[0]), y1 = std::max(y1, p[1]);
      const std::size_t pad = 3;
      eg::Geometry g;
      g.size = {static_cast<std::size_t>(std::ceil((x1 - x0) / cell)) + 2 * pad + 1, static_cast<std::size_t>(std::ceil((y1 - y0) / cell)) + 2 * pad + 1, 1};
      g.spacing = {cell, cell, 1};
      // The cells' middles stand half a cell off the room's corner, so that walls and pieces set out on the cell's lattice fall
      // between middles and are counted exactly.
      g.origin = {x0 - (static_cast<double>(pad) - 0.5) * cell, y0 - (static_cast<double>(pad) - 0.5) * cell, 0};
      eikon::distance::Mask open(g, 0);
      std::size_t floor_cells = 0, free_cells = 0;
      std::vector<std::size_t> here;
      for (std::size_t i = 0; i < pieces.size(); i++)
        if (pieces[i].storey == k && pieces[i].obstructs && out.pieces[i].room == r) here.push_back(i);
      std::vector<Ring> stairs;
      for (std::size_t i = 0; i < b.stairs.size() && i < R.stairs.size(); i++)
        if (b.stairs[i].from == k) stairs.push_back(R.stairs[i].ring);
      for (std::size_t j = 0; j < g.size[1]; j++)
        for (std::size_t i = 0; i < g.size[0]; i++) {
          const Point q{g.origin[0] + static_cast<double>(i) * cell, g.origin[1] + static_cast<double>(j) * cell};
          if (!cg::inside_ring(ring, q)) continue;
          bool hole = false;
          for (const auto& h : room.holes) hole = hole || cg::inside_ring(h, q);
          if (hole) continue;
          floor_cells++;
          bool taken = false;
          for (const std::size_t i2 : here) taken = taken || detail::in_rectangle(pieces[i2], q, -pieces[i2].width / 2, -pieces[i2].depth / 2, pieces[i2].width / 2, pieces[i2].depth / 2);
          for (const auto& st : stairs) taken = taken || cg::inside_ring(st, q);
          if (taken) continue;
          free_cells++;
          open.at(i, j, 0) = 1;
        }
      out.rooms.push_back({k, r, floor_cells ? static_cast<double>(free_cells) / static_cast<double>(floor_cells) : 1.0});
      if (here.empty()) continue;   // an empty room: nothing to reach, and its doors are judged by their swings alone

      const auto stand = eikon::distance::erode(open, radius);
      const auto parts = eikon::segmentation::components(stand, 6);
      std::vector<char> entered(parts.sizes.size() + 1, 0);
      // The component a point opens onto: the nearest cell where the disc can stand, within 0.4 m.
      const auto onto = [&](Point q) -> std::int32_t {
        const long ci = std::lround((q[0] - g.origin[0]) / cell), cj = std::lround((q[1] - g.origin[1]) / cell), span = std::lround(0.4 / cell);
        std::int32_t best = 0;
        double nearest = std::numeric_limits<double>::infinity();
        for (long j = cj - span; j <= cj + span; j++)
          for (long i = ci - span; i <= ci + span; i++) {
            if (i < 0 || j < 0 || i >= static_cast<long>(g.size[0]) || j >= static_cast<long>(g.size[1])) continue;
            const std::int32_t label = parts.labels.at(static_cast<std::size_t>(i), static_cast<std::size_t>(j), 0);
            const double d2 = static_cast<double>((i - ci) * (i - ci) + (j - cj) * (j - cj));
            if (label > 0 && d2 < nearest) nearest = d2, best = label;
          }
        return best;
      };
      bool any_way_in = false;
      for (std::size_t e = 0; e < s.walls.size(); e++) {
        const auto& side = S.sides[e];
        if (side.left != r && side.right != r) continue;
        const Point A = s.vertices[s.walls[e].a], B = s.vertices[s.walls[e].b];
        const double L = side.length;
        if (!(L > 0)) continue;
        const Point d{(B[0] - A[0]) / L, (B[1] - A[1]) / L};
        const double into = side.left == r ? 1.0 : -1.0;   // toward this room
        const Point n{-d[1] * into, d[0] * into};
        const double off = s.walls[e].thickness / 2 + radius + 2 * cell;
        const auto seed = [&](double along) { return Point{A[0] + d[0] * along + n[0] * off, A[1] + d[1] * along + n[1] * off}; };
        if (s.walls[e].kind == ap::WallKind::open) {
          any_way_in = true;
          for (double along = std::min(L / 2, radius); along < L; along += std::max(cell, radius)) {
            const std::int32_t label = onto(seed(along));
            if (label > 0) entered[static_cast<std::size_t>(label)] = 1;
          }
        }
        for (std::size_t i = 0; i < s.openings.size(); i++) {
          const auto& o = s.openings[i];
          if (o.wall != e || o.kind != ap::OpeningKind::door) continue;
          any_way_in = true;
          const std::int32_t label = onto(seed(o.at));
          if (label > 0) entered[static_cast<std::size_t>(label)] = 1;
          else
            for (auto& dr : out.doors)
              if (dr.storey == k && dr.opening == i) dr.entered = false;
        }
      }
      if (!any_way_in) continue;
      for (const std::size_t i : here) {
        const auto& p = pieces[i];
        auto& pr = out.pieces[i];
        pr.judged = true, pr.reached = false;
        // Where a person standing reaches it: its spaces grown by the radius; with none, its footprint grown a little more.
        std::vector<std::array<double, 4>> boxes;
        if (p.around > 0) boxes.push_back({-p.width / 2 - p.around - radius, -p.depth / 2 - p.around - radius, p.width / 2 + p.around + radius, p.depth / 2 + p.around + radius});
        else if (p.front > 0) boxes.push_back({-p.width / 2 - radius, p.depth / 2, p.width / 2 + radius, p.depth / 2 + p.front + radius});
        if (p.around <= 0 && p.side > 0) {
          boxes.push_back({p.width / 2, -p.depth / 2 - radius, p.width / 2 + p.side + radius, p.depth / 2 + radius});
          boxes.push_back({-p.width / 2 - p.side - radius, -p.depth / 2 - radius, -p.width / 2, p.depth / 2 + radius});
        }
        if (boxes.empty()) {
          const double m = radius + 3 * cell;
          boxes.push_back({-p.width / 2 - m, -p.depth / 2 - m, p.width / 2 + m, p.depth / 2 + m});
        }
        for (std::size_t j = 0; j < g.size[1] && !pr.reached; j++)
          for (std::size_t i2 = 0; i2 < g.size[0] && !pr.reached; i2++) {
            const std::int32_t label = parts.labels.at(i2, j, 0);
            if (label <= 0 || !entered[static_cast<std::size_t>(label)]) continue;
            const Point q{g.origin[0] + static_cast<double>(i2) * cell, g.origin[1] + static_cast<double>(j) * cell};
            for (const auto& bx : boxes)
              if (detail::in_rectangle(p, q, bx[0], bx[1], bx[2], bx[3])) { pr.reached = true; break; }
          }
      }
    }
  }
  return out;
}

// What a piece is called: its own name, or its kind.
inline std::string called(const Piece& p) { return !p.name.empty() ? p.name : !p.kind.empty() ? p.kind : "a piece"; }

}  // namespace architecture::interior
