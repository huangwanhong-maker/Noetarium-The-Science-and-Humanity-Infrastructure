// The shell of a drawn building (ADR 0097): its footprint, its roof, its faces to the eight winds, and the sun and sky at its
// glass. What a drawing's outside is, computed from the plan `plan` read.
//
// THE FOOTPRINT of a storey is what its outer walls close: each outer wall taken with the indoors on its left, the walls chained
// from end to end (at a corner where more than two meet, the leftmost turn, so that two parts touching at a corner are two rings).
// A ring that runs counter-clockwise bounds a part of the building; one that runs clockwise bounds a court within it. The ring is
// first on the walls' centrelines, then moved out to their outer faces: each edge by half its wall's thickness, consecutive lines
// met (a mitred offset, exact while the result is a simple ring that keeps its edges' directions; otherwise the centrelines are
// kept and the footprint says so).
//
// THE ROOF stands on the top storey's footprint, from the wall plate (the top of that storey's walls). Its FORM:
// - flat: the footprint itself, a little above the plate, the walls carried up about it as a parapet;
// - shed: one plane at the pitch, falling toward a bearing; its lowest wall top is at the plate;
// - hip: planes of one pitch rising from every eave: the roof of equal slope over the eaves' ring, physis's straight skeleton read
//   as a height (the lower envelope of slabs over the motorcycle graph);
// - gable: the same with some eaves standing as gables: a gable does not rise, and the wall beneath it is carried up to the roof.
// The eaves' ring is the footprint moved out by the overhang o. A point p of it stands at z = plate + tan θ (h(p) − o), h the
// roof's height at unit slope: the roof passes through the top of the walls' outer faces and its edge hangs o tan θ below the
// plate. Every sloping face has the pitch θ, so the roof's area is its plan area over cos θ, exactly.
// A gable is asked for by a point on the ground: the eave nearest it (within a metre). physis refuses a gable on an eave that is
// not an edge of the ring's convex hull or whose ends are not convex corners; what is refused is counted and noted.
// THE WALLS' RISE. Beneath a gable, a shed or a parapet the wall is carried up from the plate to the roof: along each edge of the
// footprint the roof's height above the wall's outer face, a polyline, given where it stands above the plate.
//
// THE FACES. Each outer wall of each storey looks one way: the bearing of its outward normal. The walls are gathered by the
// eight winds: gross area (length × storey height, with the rise above the plate), glass, the doors, and the share of glass.
//
// SUN AND SKY (environment's panes). Every glazed opening of an outer wall is a pane on the wall's outer face, with the eave above
// it as an overhang where its wall stands under one. The prisms about it are the neighbours given and the building's own
// footprints, each to its storey's top. A room's hours of sun are its best pane's; its average daylight factor follows from its
// panes' glass and their angles of sky, the area of its surfaces and two declared values (transmittance, reflectance).
// Left out, and said: the roof above the plate as an obstruction (the building shades as its walls do), an overhang over a gable
// or a shed's high wall, the sky's diffuse energy, light reflected from the ground and from what stands opposite.

#pragma once

#include <architecture/environment/panes.hpp>
#include <architecture/plan/plan.hpp>
#include <physis/computational_geometry/planar.hpp>
#include <physis/computational_geometry/roof.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

namespace architecture::shell {

namespace cg = physis::computational_geometry;
namespace ap = architecture::plan;
namespace ae = architecture::environment;
using Point = cg::Point2;
using Ring = std::vector<Point>;
using Point3 = std::array<double, 3>;
inline constexpr std::size_t none = static_cast<std::size_t>(-1);

// ---- the footprint -------------------------------------------------------------------------------------------------------------

struct Footprint {
  Ring centre;                      // on the walls' centrelines, the indoors on the left
  Ring ring;                        // at the walls' outer faces
  std::vector<std::vector<std::size_t>> walls;   // the walls of each edge (edge k runs from vertex k to k + 1): walls in one line and of one thickness are one edge
  std::vector<double> thickness;
  bool court = false;               // it runs clockwise: a court within the building
  bool exact = true;                // the outer faces were found; else `ring` is `centre`
};

// A ring with each edge moved by d[k] to its right (outward for a ring whose inside is on the left), consecutive lines met.
// Empty when the result is not a simple ring of the same sense with every edge keeping its direction.
inline Ring offset(const Ring& ring, const std::vector<double>& d) {
  const std::size_t n = ring.size();
  if (n < 3 || d.size() != n) return {};
  std::vector<Point> dir(n);
  for (std::size_t k = 0; k < n; k++) {
    const Point a = ring[k], b = ring[(k + 1) % n];
    const double L = std::hypot(b[0] - a[0], b[1] - a[1]);
    if (!(L > 0)) return {};
    dir[k] = {(b[0] - a[0]) / L, (b[1] - a[1]) / L};
  }
  Ring m(n);
  for (std::size_t i = 0; i < n; i++) {
    const std::size_t h = (i + n - 1) % n;
    const Point P{ring[h][0] + d[h] * dir[h][1], ring[h][1] - d[h] * dir[h][0]}, Q{ring[i][0] + d[i] * dir[i][1], ring[i][1] - d[i] * dir[i][0]};
    const double den = dir[h][0] * dir[i][1] - dir[h][1] * dir[i][0];
    if (std::fabs(den) < 1e-9) {
      m[i] = Q;
    } else {
      const double s = ((Q[0] - P[0]) * dir[i][1] - (Q[1] - P[1]) * dir[i][0]) / den;
      m[i] = {P[0] + s * dir[h][0], P[1] + s * dir[h][1]};
    }
  }
  if (cg::ring_area(m) * cg::ring_area(ring) <= 0) return {};
  for (std::size_t k = 0; k < n; k++) {
    const Point a = m[k], b = m[(k + 1) % n];
    if (!((b[0] - a[0]) * dir[k][0] + (b[1] - a[1]) * dir[k][1] > 0)) return {};
  }
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = i + 2; j < n; j++) {
      if (i == 0 && j == n - 1) continue;
      if (cg::segments_cross(m[i], m[(i + 1) % n], m[j], m[(j + 1) % n])) return {};
    }
  return m;
}

inline std::vector<Footprint> footprints(const ap::Storey& s, const ap::StoreyReading& R) {
  std::vector<Footprint> out;
  if (!R.sound) return out;
  const auto indoor = [&](std::size_t r) { return r != ap::none && ap::use_of(R.rooms[r].kind).indoor; };
  struct Arc { std::size_t from, to, wall; bool used = false; };
  std::vector<Arc> arcs;
  for (std::size_t i = 0; i < s.walls.size() && i < R.sides.size(); i++) {
    if (!R.sides[i].outer) continue;
    const auto& w = s.walls[i];
    if (indoor(R.sides[i].left)) arcs.push_back({w.a, w.b, i});
    else arcs.push_back({w.b, w.a, i});
  }
  std::multimap<std::size_t, std::size_t> leaving;
  for (std::size_t k = 0; k < arcs.size(); k++) leaving.insert({arcs[k].from, k});
  const auto& V = s.vertices;
  for (std::size_t start = 0; start < arcs.size(); start++) {
    if (arcs[start].used) continue;
    Footprint f;
    std::size_t k = start;
    bool closed = false;
    for (std::size_t guard = 0; guard <= arcs.size(); guard++) {
      arcs[k].used = true;
      f.centre.push_back(V[arcs[k].from]);
      f.walls.push_back({arcs[k].wall});
      f.thickness.push_back(s.walls[arcs[k].wall].thickness);
      if (arcs[k].to == arcs[start].from) { closed = true; break; }
      // Onward from the arc's end: the leftmost turn among the arcs not yet walked.
      const Point a = V[arcs[k].from], b = V[arcs[k].to];
      const double in = std::atan2(b[1] - a[1], b[0] - a[0]);
      std::size_t next = none;
      double best = -10;
      const auto range = leaving.equal_range(arcs[k].to);
      for (auto it = range.first; it != range.second; ++it) {
        if (arcs[it->second].used) continue;
        const Point c = V[arcs[it->second].to];
        double turn = std::atan2(c[1] - b[1], c[0] - b[0]) - in;
        while (turn > std::numbers::pi) turn -= 2 * std::numbers::pi;
        while (turn <= -std::numbers::pi) turn += 2 * std::numbers::pi;
        if (turn > best) best = turn, next = it->second;
      }
      if (next == none) break;
      k = next;
    }
    if (!closed || f.centre.size() < 3) continue;
    // Walls in one line and of one thickness are one edge of the footprint.
    for (bool merged = true; merged && f.centre.size() > 3;) {
      merged = false;
      const std::size_t m = f.centre.size();
      for (std::size_t i = 0; i < m; i++) {
        const std::size_t h = (i + m - 1) % m;
        const Point a = f.centre[h], c = f.centre[i], e = f.centre[(i + 1) % m];
        const double ux = c[0] - a[0], uy = c[1] - a[1], vx = e[0] - c[0], vy = e[1] - c[1];
        if (std::fabs(ux * vy - uy * vx) > 1e-9 * (std::hypot(ux, uy) * std::hypot(vx, vy)) || ux * vx + uy * vy <= 0 || std::fabs(f.thickness[h] - f.thickness[i]) > 1e-9) continue;
        f.walls[h].insert(f.walls[h].end(), f.walls[i].begin(), f.walls[i].end());
        f.centre.erase(f.centre.begin() + i), f.walls.erase(f.walls.begin() + i), f.thickness.erase(f.thickness.begin() + i);
        merged = true;
        break;
      }
    }
    f.court = cg::ring_area(f.centre) < 0;
    std::vector<double> half(f.thickness.size());
    for (std::size_t e = 0; e < half.size(); e++) half[e] = f.thickness[e] / 2;
    f.ring = offset(f.centre, half);
    if (f.ring.empty()) f.ring = f.centre, f.exact = false;
    out.push_back(std::move(f));
  }
  return out;
}

// ---- the roof ------------------------------------------------------------------------------------------------------------------

struct RoofSpec {
  std::string form = "flat";        // flat, shed, gable, hip
  double pitch = 0;                 // radians
  double eaves = 0;                 // the overhang beyond the walls' outer faces (m)
  double parapet = 0.3;             // flat: the walls' rise about the roof (m)
  double fall = -std::numbers::pi / 2;  // shed: the bearing the water runs toward, radians counter-clockwise from east
  std::vector<Point> gables;        // points on the ground; the eave nearest each is a gable
};

struct Face { std::size_t edge = none; std::vector<Point3> ring; };   // a convex piece of a face; `edge` the eave it rises from
struct Line { Point3 a{}, b{}; int kind = 0; };                       // 0 a ridge, 1 a hip, 2 a valley
struct Rise {
  std::size_t edge = none;                         // of the footprint
  Point a{}, b{};                                  // that edge, at the walls' outer face
  double thickness = 0;
  std::vector<std::pair<double, double>> top;      // (distance from a, height): the roof above the wall
  double area = 0;                                 // of wall above the plate
};

struct Roof {
  std::size_t storey = 0, part = 0;
  std::string form;
  double pitch = 0, eaves = 0, plate = 0, top = 0;
  double area = 0, plan_area = 0, wall_area = 0;   // the roof's surface, its plan, the walls carried up to it
  std::vector<Face> faces;
  std::vector<Line> lines;
  std::vector<Point3> outline;                     // the roof's edge, round
  std::vector<Rise> rises;
  std::vector<char> gable;                         // of each edge of the footprint
  std::size_t refused = 0;                         // gables asked for and not made
  bool exact = true;
  std::string note;
};

namespace detail {

inline double segment_distance(Point p, Point a, Point b) { return cg::segment_distance(p, a, b); }

// Where the segment a–b lies within a convex ring: the interval of its parameter, or first > second for none.
inline std::pair<double, double> within(const Ring& ring, Point a, Point b) {
  double lo = 0, hi = 1;
  const double sense = cg::ring_area(ring) >= 0 ? 1 : -1;
  const std::size_t n = ring.size();
  for (std::size_t k = 0; k < n; k++) {
    const Point p = ring[k], q = ring[(k + 1) % n];
    const double ex = q[0] - p[0], ey = q[1] - p[1], L = std::hypot(ex, ey);
    if (!(L > 0)) continue;
    const double fa = sense * (ex * (a[1] - p[1]) - ey * (a[0] - p[0])) / L, fb = sense * (ex * (b[1] - p[1]) - ey * (b[0] - p[0])) / L;
    constexpr double eps = 1e-9;
    if (fa < -eps && fb < -eps) return {1, 0};
    if (fa < -eps) lo = std::max(lo, fa / (fa - fb));
    else if (fb < -eps) hi = std::min(hi, fa / (fa - fb));
  }
  return {lo, hi};
}

inline void tidy(std::vector<std::pair<double, double>>& top) {
  std::sort(top.begin(), top.end());
  std::vector<std::pair<double, double>> out;
  for (const auto& p : top) {
    if (!out.empty() && std::fabs(p.first - out.back().first) < 1e-7) { out.back().second = std::max(out.back().second, p.second); continue; }
    out.push_back(p);
  }
  // Points in line with their neighbours say nothing.
  std::vector<std::pair<double, double>> lean;
  for (std::size_t i = 0; i < out.size(); i++) {
    if (i > 0 && i + 1 < out.size()) {
      const auto& a = lean.back();
      const auto& c = out[i + 1];
      const double z = a.second + (c.second - a.second) * (out[i].first - a.first) / (c.first - a.first);
      if (std::fabs(z - out[i].second) < 1e-7) continue;
    }
    lean.push_back(out[i]);
  }
  top = std::move(lean);
}

inline double trapezia(const std::vector<std::pair<double, double>>& top, double plate) {
  double area = 0;
  for (std::size_t i = 0; i + 1 < top.size(); i++) area += (top[i + 1].first - top[i].first) * (std::max(0.0, top[i].second - plate) + std::max(0.0, top[i + 1].second - plate)) / 2;
  return area;
}

}  // namespace detail

// The roof over one part of a storey's footprint, from a plate at height `plate`.
inline Roof roof(const Footprint& f, const RoofSpec& spec, double plate) {
  Roof R;
  R.form = spec.form, R.plate = plate, R.top = plate;
  const std::size_t n = f.ring.size();
  R.gable.assign(n, 0);
  const double tan = std::tan(spec.pitch), cos = std::cos(spec.pitch);
  const auto edge = [&](std::size_t k) { return std::pair<Point, Point>{f.ring[k], f.ring[(k + 1) % n]}; };
  const auto rise = [&](std::size_t k, std::vector<std::pair<double, double>> top) {
    detail::tidy(top);
    const double area = detail::trapezia(top, plate);
    if (!(area > 1e-9)) return;
    const auto [a, b] = edge(k);
    R.rises.push_back({k, a, b, f.thickness[k], std::move(top), area});
    R.wall_area += area;
  };
  if (!f.exact) R.exact = false, R.note = "the footprint is taken on the walls' centrelines";

  if (spec.form == "flat" || !(spec.pitch > 1e-6)) {
    R.form = "flat";
    const double z = plate + 0.03;
    for (const auto& t : cg::triangulate_ring(f.ring)) R.faces.push_back({none, {Point3{f.ring[t[0]][0], f.ring[t[0]][1], z}, Point3{f.ring[t[1]][0], f.ring[t[1]][1], z}, Point3{f.ring[t[2]][0], f.ring[t[2]][1], z}}});
    for (const Point& p : f.ring) R.outline.push_back({p[0], p[1], z});
    R.plan_area = R.area = std::fabs(cg::ring_area(f.ring));
    if (spec.parapet > 0)
      for (std::size_t k = 0; k < n; k++) {
        const auto [a, b] = edge(k);
        rise(k, {{0.0, plate + spec.parapet}, {ap::distance(a, b), plate + spec.parapet}});
      }
    R.top = plate + std::max(0.0, spec.parapet);
    return R;
  }

  // The eaves' ring: the footprint moved out by the overhang.
  R.pitch = spec.pitch, R.eaves = spec.eaves;
  Ring eaves = spec.eaves > 0 ? offset(f.ring, std::vector<double>(n, f.court ? -spec.eaves : spec.eaves)) : f.ring;
  if (eaves.empty()) {
    eaves = f.ring, R.eaves = 0, R.exact = false;
    R.note += std::string(R.note.empty() ? "" : "; ") + "the eaves could not be set out from this footprint, and the roof ends at the walls";
  }
  const double o = R.eaves;

  if (spec.form == "shed") {
    // Up the slope: against the fall. The lowest point of the walls is at the plate.
    const Point up{-std::cos(spec.fall), -std::sin(spec.fall)};
    double low = std::numeric_limits<double>::infinity();
    for (const Point& p : f.ring) low = std::min(low, p[0] * up[0] + p[1] * up[1]);
    const auto z = [&](Point p) { return plate + tan * (p[0] * up[0] + p[1] * up[1] - low); };
    for (const auto& t : cg::triangulate_ring(eaves)) R.faces.push_back({none, {Point3{eaves[t[0]][0], eaves[t[0]][1], z(eaves[t[0]])}, Point3{eaves[t[1]][0], eaves[t[1]][1], z(eaves[t[1]])}, Point3{eaves[t[2]][0], eaves[t[2]][1], z(eaves[t[2]])}}});
    for (const Point& p : eaves) R.outline.push_back({p[0], p[1], z(p)}), R.top = std::max(R.top, z(p));
    R.plan_area = std::fabs(cg::ring_area(eaves)), R.area = R.plan_area / cos;
    for (std::size_t k = 0; k < n; k++) {
      const auto [a, b] = edge(k);
      rise(k, {{0.0, z(a)}, {ap::distance(a, b), z(b)}});
    }
    return R;
  }

  // A hip, or a gable: the roof of equal slope over the eaves' ring.
  std::vector<char> gable(n, 0);
  std::size_t asked = 0;
  if (spec.form == "gable")
    for (const Point& g : spec.gables) {
      std::size_t best = none;
      double d = 1.0;
      for (std::size_t k = 0; k < n; k++) {
        const auto [a, b] = edge(k);
        const double here = detail::segment_distance(g, a, b);
        if (here < d) d = here, best = k;
      }
      if (best != none && !gable[best]) gable[best] = 1, asked++;
    }
  const cg::Roof S = cg::roof({eaves}, gable);
  std::size_t made = 0;
  for (std::size_t k = 0; k < n; k++) R.gable[k] = S.edges.size() == n && S.edges[k].gable ? 1 : 0, made += R.gable[k] ? 1 : 0;
  R.refused = asked - std::min(asked, made);
  if (!S.exact) R.exact = false;
  if (!S.note.empty()) R.note += std::string(R.note.empty() ? "" : "; ") + S.note;
  if (R.refused) R.note += std::string(R.note.empty() ? "" : "; ") + "a gable stands only on an eave of the footprint's hull between two convex corners: " + std::to_string(R.refused) + " asked for were made hips";
  const auto lift = [&](std::size_t e, Point p) { return Point3{p[0], p[1], plate + tan * (S.plane(e, p) - o)}; };
  for (const auto& piece : S.pieces) {
    Face face;
    face.edge = piece.edge;
    for (const Point& p : piece.ring) face.ring.push_back(lift(piece.edge, p)), R.top = std::max(R.top, face.ring.back()[2]);
    R.plan_area += std::fabs(cg::ring_area(piece.ring));
    R.faces.push_back(std::move(face));
  }
  R.area = R.plan_area / cos;
  for (const auto& l : S.lines) R.lines.push_back({{l.a[0], l.a[1], plate + tan * (l.za - o)}, {l.b[0], l.b[1], plate + tan * (l.zb - o)}, l.kind});
  // Along a segment, the roof above it: where it passes from one piece to the next, and how high it stands there.
  const auto profile = [&](Point a, Point b) {
    std::vector<std::pair<double, double>> top;
    const double L = ap::distance(a, b);
    for (const auto& piece : S.pieces) {
      const auto [lo, hi] = detail::within(piece.ring, a, b);
      if (hi - lo < 1e-9) continue;
      for (const double t : {lo, hi}) {
        const Point p{a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t};
        top.push_back({t * L, plate + tan * (S.plane(piece.edge, p) - o)});
      }
    }
    detail::tidy(top);
    return top;
  };
  // The roof's edge, with the points at which a gable's verge turns.
  for (std::size_t k = 0; k < n; k++) {
    const Point a = eaves[k], b = eaves[(k + 1) % n];
    const double L = ap::distance(a, b);
    // A little within the ring, so that each point of the edge lies in a piece.
    const Point in{-(b[1] - a[1]) / L * 1e-6 * (f.court ? -1 : 1), (b[0] - a[0]) / L * 1e-6 * (f.court ? -1 : 1)};
    const auto top = profile({a[0] + in[0], a[1] + in[1]}, {b[0] + in[0], b[1] + in[1]});
    if (top.size() < 2) { R.outline.push_back({a[0], a[1], plate - tan * o}); continue; }
    for (std::size_t i = 0; i + 1 < top.size(); i++) R.outline.push_back({a[0] + (b[0] - a[0]) * top[i].first / L, a[1] + (b[1] - a[1]) * top[i].first / L, top[i].second});
  }
  // The walls carried up beneath the gables.
  for (std::size_t k = 0; k < n; k++) {
    if (!R.gable[k]) continue;
    const auto [a, b] = edge(k);
    rise(k, profile(a, b));
  }
  return R;
}

// ---- the faces to the winds ----------------------------------------------------------------------------------------------------

struct Facade {
  int wind = 0;                 // 0 east, 1 north-east, … 7 south-east: the outward bearing to the nearest 45°
  double wall = 0, glass = 0, length = 0;
  std::size_t windows = 0, doors = 0;
};

// One glazed opening as a pane, and what was read of it.
struct Glass {
  std::size_t storey = 0, opening = 0, room = none;
  double area = 0;              // of glass
  double bearing = 0;           // the way it looks, radians counter-clockwise from east
  ae::Pane pane;
  ae::PaneReading sun;
};

struct RoomLight {
  std::size_t storey = 0, room = 0;
  double glass = 0, surfaces = 0, sun_hours = 0, energy = 0, daylight_factor = 0, sky = 0;   // sky: the glass-weighted mean angle, radians
  std::size_t panes = 0;
};

struct Light {
  double transmittance = 0.68, reflectance = 0.5, gain = 0.6;   // the glass's diffuse transmittance; the rooms' mean reflectance; the glass's solar gain
};

struct Shell {
  std::vector<std::vector<Footprint>> footprints;   // by storey
  std::vector<Roof> roofs;
  std::array<Facade, 8> facades{};
  std::vector<Glass> glass;
  std::vector<RoomLight> rooms;
  double solar_gain = 0;       // through all the glass on the day given, on a clear day: Wh
  bool lit = false;            // whether instants of the Sun were given
};

inline Shell read(const ap::Building& b, const ap::Reading& R, const RoofSpec& spec, const std::vector<ae::Sun>& suns = {}, const std::vector<ae::Prism>& around = {}, const Light& light = {}) {
  Shell S;
  const std::size_t K = b.storeys.size();
  S.footprints.resize(K);
  std::vector<double> base(K + 1, 0.0);
  for (std::size_t k = 0; k < K; k++) base[k + 1] = base[k] + b.storeys[k].height;
  std::size_t top = none;
  for (std::size_t k = 0; k < K; k++) {
    if (k >= R.storeys.size()) break;
    S.footprints[k] = footprints(b.storeys[k], R.storeys[k]);
    if (!S.footprints[k].empty()) top = k;
  }
  if (top != none) {
    std::size_t part = 0;
    for (const Footprint& f : S.footprints[top]) {
      if (f.court) continue;
      Roof roof_ = roof(f, spec, base[top + 1]);
      roof_.storey = top, roof_.part = part++;
      S.roofs.push_back(std::move(roof_));
    }
  }
  // The building's own prisms, and the eave over each wall of the top storey.
  std::vector<ae::Prism> prisms = around;
  for (std::size_t k = 0; k < K; k++)
    for (const Footprint& f : S.footprints[k])
      if (!f.court) prisms.push_back({f.ring, base[k + 1], -1});
  std::map<std::size_t, std::pair<double, double>> eave;   // wall of the top storey → (overhang, its underside)
  if (top != none) {
    std::size_t at = 0;
    for (const Footprint& f : S.footprints[top]) {
      if (f.court) continue;
      const Roof& roof_ = S.roofs[at++];
      if (!(roof_.eaves > 0) || roof_.form == "flat") continue;
      for (std::size_t e = 0; e < f.walls.size(); e++) {
        bool rises = false;
        for (const Rise& r : roof_.rises) rises = rises || (r.edge == e && r.area > 1e-6);
        if (!rises)
          for (const std::size_t w : f.walls[e]) eave[w] = {roof_.eaves, roof_.plate - std::tan(roof_.pitch) * roof_.eaves - 0.1};
      }
    }
  }
  std::vector<ae::Pane> panes;
  for (std::size_t k = 0; k < K && k < R.storeys.size(); k++) {
    const ap::Storey& s = b.storeys[k];
    const ap::StoreyReading& r = R.storeys[k];
    if (!r.sound) continue;
    const auto indoor = [&](std::size_t room) { return room != ap::none && ap::use_of(r.rooms[room].kind).indoor; };
    for (std::size_t i = 0; i < s.walls.size() && i < r.sides.size(); i++) {
      if (!r.sides[i].outer) continue;
      const Point a = s.vertices[s.walls[i].a], c = s.vertices[s.walls[i].b];
      const double L = ap::distance(a, c);
      if (!(L > 0)) continue;
      const Point dir{(c[0] - a[0]) / L, (c[1] - a[1]) / L};
      const bool left = indoor(r.sides[i].left);
      const Point out = left ? Point{dir[1], -dir[0]} : Point{-dir[1], dir[0]};
      const double bearing = std::atan2(out[1], out[0]);
      const int wind = ((static_cast<int>(std::lround(bearing / (std::numbers::pi / 4))) % 8) + 8) % 8;
      Facade& F = S.facades[wind];
      F.wall += L * s.height, F.length += L;
      for (std::size_t o = 0; o < s.openings.size(); o++) {
        const ap::Opening& op = s.openings[o];
        if (op.wall != i) continue;
        const double height = std::min(op.height, s.height - op.sill), area = op.width * std::max(0.0, height) * op.glazed;
        if (op.kind == ap::OpeningKind::door) F.doors++;
        else F.windows++;
        if (!(area > 0)) continue;
        F.glass += area;
        Glass g;
        g.storey = k, g.opening = o, g.room = left ? r.sides[i].left : r.sides[i].right, g.area = area, g.bearing = bearing;
        const double half = s.walls[i].thickness / 2;
        g.pane.x = a[0] + dir[0] * op.at + out[0] * half, g.pane.y = a[1] + dir[1] * op.at + out[1] * half;
        g.pane.z0 = base[k] + op.sill, g.pane.z1 = base[k] + op.sill + height, g.pane.width = op.width, g.pane.nx = out[0], g.pane.ny = out[1];
        if (k == top) {
          const auto it = eave.find(i);
          if (it != eave.end()) g.pane.overhang = it->second.first, g.pane.overhang_z = it->second.second;
        }
        panes.push_back(g.pane);
        S.glass.push_back(std::move(g));
      }
    }
  }
  for (const Roof& roof_ : S.roofs)
    for (const Rise& r : roof_.rises) {
      const double bearing = std::atan2(-(r.b[0] - r.a[0]), r.b[1] - r.a[1]);   // the right of a → b
      S.facades[((static_cast<int>(std::lround(bearing / (std::numbers::pi / 4))) % 8) + 8) % 8].wall += r.area;
    }
  S.lit = !suns.empty();
  const auto readings = ae::panes(panes, suns, prisms);
  for (std::size_t g = 0; g < S.glass.size(); g++) S.glass[g].sun = readings[g];
  // The rooms.
  for (std::size_t k = 0; k < K && k < R.storeys.size(); k++) {
    const ap::StoreyReading& r = R.storeys[k];
    if (!r.sound) continue;
    for (std::size_t room = 0; room < r.rooms.size(); room++) {
      if (!ap::use_of(r.rooms[room].kind).indoor) continue;
      RoomLight L;
      L.storey = k, L.room = room;
      const auto& rm = r.rooms[room];
      const double floor = rm.net_area > 0 ? rm.net_area : rm.area;
      const double round = rm.net_ring.size() >= 3 ? [&] { double p = 0; for (std::size_t i = 0; i < rm.net_ring.size(); i++) p += ap::distance(rm.net_ring[i], rm.net_ring[(i + 1) % rm.net_ring.size()]); return p; }() : rm.perimeter;
      L.surfaces = 2 * floor + round * b.storeys[k].ceiling;
      double glass_sky = 0;
      for (const Glass& g : S.glass) {
        if (g.storey != k || g.room != room) continue;
        L.panes++, L.glass += g.area;
        glass_sky += g.area * g.sun.sky * 180 / std::numbers::pi;
        L.sun_hours = std::max(L.sun_hours, g.sun.full_hours);
        L.energy += g.sun.energy * g.area;
      }
      L.sky = L.glass > 0 ? glass_sky / L.glass * std::numbers::pi / 180 : 0;
      L.daylight_factor = ae::daylight_factor(glass_sky, L.surfaces, light.transmittance, light.reflectance);
      S.solar_gain += L.energy * light.gain;
      S.rooms.push_back(L);
    }
  }
  return S;
}

}  // namespace architecture::shell
