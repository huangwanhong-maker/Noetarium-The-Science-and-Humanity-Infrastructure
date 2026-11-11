// What could be built on a block (ADR 0089), as one composition any work may call: the Urban Science Workbench's plug-in
// "Architecture" answers its page with it, and a study may run it on a city model without the page. A block is the face of the
// streets about a point; its buildings are its lots (parcels are not in the open data, so a lot is a footprint as built); the rules
// are the caller's, declared. A design gives each lot a podium, an upper part on a share of its upper envelope and their storeys,
// and may open one passage through the block; heuresis's NSGA-III searches the designs, each measured by the architecture domain
// (rules, form, frontage, sun and shade, the day's heat) and by urban science (the streets' configuration, the floor within a walk).
// Nothing in it names a city. Like the toolbox (toolbox.hpp, ADR 0080), it stands in the API so that no work depends on another.
//
//   Designer::design  {at: [x, y], streets: [[x, y, …]], ways: [[x, y, …]], buildings: [[height, x0, y0, x1, y1, …]], rules: {…},
//                      suns: [[altitude, azimuth, hours, air °C]], night: {air, hours}, generations?, seed?, least_floor?}
//                         the block, its lots and passages, the designs as built and on the front, each measured
//   Designer::sunmap  {design}   the hours of sun on the open ground about the block, for a design of the last front (−1: as built)
//
// Coordinates are metres of a local frame (east, north); angles radians, azimuth from north through east.

#pragma once

#include <architecture/environment/balance.hpp>
#include <architecture/environment/sun.hpp>
#include <architecture/form/form.hpp>
#include <architecture/frontage/frontage.hpp>
#include <architecture/rules/rules.hpp>
#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>
#include <urban/syntax/configuration.hpp>
#include <urban/syntax/syntax.hpp>

#include <calx/scheduler/thread_pool.hpp>
#include <heuresis/multiobjective/multiobjective.hpp>
#include <heuresis/search/search.hpp>
#include <heuresis/variation/variation.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numbers>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

#include <science/json.hpp>

namespace science::design {


namespace cg = physis::computational_geometry;
namespace ar = architecture::rules;
namespace af = architecture::form;
namespace ae = architecture::environment;
namespace fr = architecture::frontage;
namespace ua = urban::access;
namespace hs = heuresis::search;
namespace hv = heuresis::variation;
namespace mo = heuresis::multiobjective;
using Ring = std::vector<cg::Point2>;

constexpr double LEVELS[4] = {0.25, 0.5, 0.75, 1.0};  // the shares of the upper envelope an upper part may cover
constexpr double SPACING = 4, GROUND_MARGIN = 15, NEIGHBOUR_REACH = 40, WALK = 1.34, WALK_LIMIT = 300, PASSAGE = 6, SCORE_RADIUS = 300, SYNTAX_RADIUS = 800;

inline std::size_t threads() { return std::clamp<std::size_t>(std::thread::hardware_concurrency(), 1, 8); }
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline Json fixed(double v, double scale = 10) { return std::isfinite(v) ? Json(std::round(v * scale) / scale) : Json(); }
inline double area(const Ring& r) { return std::fabs(cg::ring_area(r)); }
inline Ring ccw(Ring r) { if (cg::ring_area(r) < 0) std::reverse(r.begin(), r.end()); return r; }
inline cg::Point2 centroid(const Ring& r) { return af::centroid(r); }

inline double ring_distance(const Ring& r, cg::Point2 p) {
  double d = std::numeric_limits<double>::infinity();
  for (std::size_t k = 0; k < r.size(); k++) d = std::min(d, cg::segment_distance(p, r[k], r[(k + 1) % r.size()]));
  return d;
}

inline std::vector<std::vector<double>> polylines(const Json& j) {
  std::vector<std::vector<double>> out;
  for (std::size_t k = 0; k < j.size(); k++) {
    std::vector<double> xy;
    for (std::size_t i = 0; i < j[k].size(); i++) xy.push_back(j[k][i].as_number());
    if (xy.size() >= 4) out.push_back(std::move(xy));
  }
  return out;
}

inline Json ring_json(const Ring& r) {
  Json a = Json::array();
  for (const auto& p : r) a.push(fixed(p[0], 100)), a.push(fixed(p[1], 100));
  return a;
}

// ---- the block -----------------------------------------------------------------------------------------------------------------

// The face of the streets' plane graph about a point: the least bounded face holding it.
inline Ring block_about(const std::vector<std::vector<double>>& streets, cg::Point2 at) {
  std::vector<ua::Way> w;
  for (const auto& xy : streets) w.push_back({xy, 1.0});
  const auto n = ua::network_of(w, 1.0);
  std::vector<cg::Point2> pts(n.x.size());
  for (std::size_t p = 0; p < pts.size(); p++) pts[p] = {n.x[p], n.y[p]};
  std::vector<cg::PlaneEdge> edges;
  for (std::size_t u = 0; u < n.arcs.size(); u++)
    for (const auto& a : n.arcs[u])
      if (u < a.to) edges.push_back({u, a.to, {}});
  // A pair of points joined twice (two ways along one line) is one edge.
  std::sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) { return a.u != b.u ? a.u < b.u : a.v < b.v; });
  edges.erase(std::unique(edges.begin(), edges.end(), [](const auto& a, const auto& b) { return a.u == b.u && a.v == b.v; }), edges.end());
  const auto faces = cg::plane_faces(pts, edges);
  Ring best;
  double least = std::numeric_limits<double>::infinity();
  for (const auto& f : faces) {
    if (!(f.area > 0) || f.area >= least || f.area > 4e5) continue;
    Ring r;
    for (auto v : f.vertices) r.push_back(pts[v]);
    if (cg::inside_ring(r, at)) best = std::move(r), least = f.area;
  }
  if (best.empty()) throw std::runtime_error("the point picked lies in no block of the streets drawn here: pick within a block, nearer");
  // Spikes (a dead end walked out and back) are left: they enclose nothing.
  return best;
}

// The passages through a block: across its middle, perpendicular to its long axis, and along it; each the segment of the line
// through the block's centroid between the two points where it meets the block's boundary.
inline std::vector<std::array<cg::Point2, 2>> passages_of(const Ring& block) {
  const auto c = centroid(block);
  double sxx = 0, syy = 0, sxy = 0;
  for (std::size_t k = 0; k < block.size(); k++) {
    const double x = block[k][0] - c[0], y = block[k][1] - c[1];
    sxx += x * x, syy += y * y, sxy += x * y;
  }
  const double theta = 0.5 * std::atan2(2 * sxy, sxx - syy);  // the long axis
  std::vector<std::array<cg::Point2, 2>> out;
  for (double a : {theta + std::numbers::pi / 2, theta}) {
    const cg::Point2 u{std::cos(a), std::sin(a)}, v{-u[0], -u[1]};
    const double t1 = cg::ray_entry(block, c, u), t2 = cg::ray_entry(block, c, v);
    if (!std::isfinite(t1) || !std::isfinite(t2) || t1 + t2 < 20) continue;
    out.push_back({cg::Point2{c[0] + t2 * v[0], c[1] + t2 * v[1]}, cg::Point2{c[0] + t1 * u[0], c[1] + t1 * u[1]}});
  }
  return out;
}

// The ways with a passage's ends set into the segments they fall on, and the passage added.
inline std::vector<std::vector<double>> with_passage(std::vector<std::vector<double>> ways, const std::array<cg::Point2, 2>& passage) {
  for (const auto& p : passage) {
    double best = 0.75;
    std::size_t bw = 0, bk = 0;
    bool found = false;
    for (std::size_t w = 0; w < ways.size(); w++)
      for (std::size_t k = 0; k + 3 < ways[w].size(); k += 2) {
        const double d = cg::segment_distance(p, {ways[w][k], ways[w][k + 1]}, {ways[w][k + 2], ways[w][k + 3]});
        if (d < best) best = d, bw = w, bk = k, found = true;
      }
    if (found) ways[bw].insert(ways[bw].begin() + static_cast<std::ptrdiff_t>(bk + 2), {p[0], p[1]});
  }
  ways.push_back({passage[0][0], passage[0][1], passage[1][0], passage[1][1]});
  return ways;
}

// ---- the site ------------------------------------------------------------------------------------------------------------------

struct Building {
  Ring ring;
  double height = 0;
};

struct Wall {
  std::size_t receiver = 0;
  int piece = 0;
  int part = -1;  // −1 the podium; c the upper part on LEVELS[c]
  double z = 0, area = 0;
};

// A block as one variant: without a passage or with one; or as built.
struct Variant {
  std::string name;
  bool built = false;
  int passage = -1;
  std::vector<ar::Lot> pieces;
  std::vector<int> lot_of;                     // piece → lot
  std::vector<int> podium_var;                 // piece → variable (−1: nothing can stand)
  std::vector<std::array<int, 4>> upper_var;   // piece → variable for each level (−1: none)
  std::vector<ar::Envelope> envelope;          // piece → its envelopes
  std::vector<Ring> ground_env;                // piece → its ground envelope
  std::vector<std::array<Ring, 4>> upper_ring; // piece → the upper part on each level
  std::size_t variables = 0;
  ae::Shading S;
  ae::Cover C;
  std::vector<std::size_t> ground, neighbour;  // receivers
  std::vector<Wall> walls;
  double ground_cell = SPACING * SPACING, neighbour_cell = SPACING * SPACING;
  double configuration = 0, passage_nach = std::numeric_limits<double>::quiet_NaN();
  std::vector<double> reach;                   // piece → the floor within a walk of its door (m²)
};

struct Measures {
  double floor = 0, street_sun = 0, facade_sun = 0, heat = 0, configuration = 0, access = 0, frontage = 0, height = 0;
  double need = 0;
};

struct Design {
  hv::Integers podium, upper, level;  // per lot
  std::int64_t passage = 0;           // 0 none, k the passage k − 1
};
using Individual = hs::Individual<Design>;

struct Site {
  Ring block;
  std::vector<ar::Lot> lots;
  std::vector<double> lot_height;
  std::vector<Building> context;
  std::vector<std::array<cg::Point2, 2>> passages;
  ar::Rules rules;
  double min_frontage = 0, least_floor = 0.5;
  std::vector<ae::Sun> suns;
  ae::Day day;
  std::vector<Variant> variants;  // [0] as built, [1] no passage, [2…] a passage each
  Measures as_built;
  int podium_max = 0, upper_max = 0;
  std::vector<std::size_t> order;  // lots along the block's long axis, for crossover
  std::vector<Individual> front;
};

// The receivers of a variant and their thresholds; `parts` the prisms that stand on the block in it.
// `owners[k]` names the piece of parts[k] and which part it is (−1 the podium, c the upper part on LEVELS[c]).
inline void receive(Site& site, Variant& v, const std::vector<ae::Prism>& parts, const std::vector<std::array<int, 2>>& owners) {
  std::vector<ae::Prism> prisms;
  for (const auto& b : site.context) prisms.push_back({b.ring, b.height, -1});
  const std::size_t base = prisms.size();
  for (const auto& p : parts) prisms.push_back(p);
  std::vector<ae::Receiver> R;
  // The open ground: within the block or a margin about it, under nothing that stands or may stand.
  double x0 = 1e300, y0 = 1e300, x1 = -1e300, y1 = -1e300;
  for (const auto& q : site.block) x0 = std::min(x0, q[0]), y0 = std::min(y0, q[1]), x1 = std::max(x1, q[0]), y1 = std::max(y1, q[1]);
  for (double y = y0 - GROUND_MARGIN + SPACING / 2; y < y1 + GROUND_MARGIN; y += SPACING)
    for (double x = x0 - GROUND_MARGIN + SPACING / 2; x < x1 + GROUND_MARGIN; x += SPACING) {
      const cg::Point2 p{x, y};
      if (!cg::inside_ring(site.block, p) && ring_distance(site.block, p) > GROUND_MARGIN) continue;
      bool under = false;
      for (const auto& pr : prisms) under = under || (pr.ring.size() >= 3 && (pr.variable >= 0 || pr.height > 0) && cg::inside_ring(pr.ring, p));
      if (under) continue;
      v.ground.push_back(R.size());
      R.push_back({x, y, 0, 0, 0, -1});
    }
  // The faces of the neighbours across the streets.
  for (std::size_t c = 0; c < site.context.size(); c++) {
    const auto& b = site.context[c];
    if (ring_distance(site.block, centroid(b.ring)) > NEIGHBOUR_REACH + 30) continue;
    for (auto& r : ae::faces(b.ring, b.height, static_cast<int>(c), SPACING, SPACING)) {
      if (ring_distance(site.block, {r.x, r.y}) > NEIGHBOUR_REACH && !cg::inside_ring(site.block, {r.x, r.y})) continue;
      v.neighbour.push_back(R.size());
      R.push_back(r);
    }
  }
  // The block's own walls: the podium's to the most it may rise, each upper part's to the height limit; or as built.
  const double rise = 2 * site.rules.storey;
  for (std::size_t k = 0; k < parts.size(); k++) {
    const auto& p = parts[k];
    if (p.ring.size() < 3) continue;
    const int piece = owners[k][0], part = owners[k][1];
    const double top = v.built ? p.height : (part < 0 ? site.podium_max * site.rules.storey : site.rules.max_height);
    for (auto& r : ae::faces(p.ring, std::min(top, 400.0), static_cast<int>(base + k), SPACING, rise)) {
      v.walls.push_back({R.size(), piece, part, r.z, SPACING * rise});
      R.push_back(r);
    }
  }
  double reach = 0;
  for (const auto& s : site.suns) reach = std::max(reach, std::min(600.0, 150.0 / std::tan(std::max(s.altitude, 0.05))));
  v.S = ae::shading(std::move(R), site.suns, prisms, std::max(reach, 100.0));
  v.C = ae::covering(v.S.receivers, prisms);
}

// The heights each variable takes in a design, the massing of each piece, and its measures.
inline Measures measure(const Site& site, const Variant& v, const Design& d, std::vector<double>* heights_out = nullptr, std::vector<ar::Massing>* massings = nullptr) {
  Measures M;
  std::vector<double> h(v.variables, 0.0);
  std::vector<double> podium_h(v.pieces.size(), 0), top(v.pieces.size(), 0);
  std::vector<int> chosen(v.pieces.size(), -1);
  double wall = 0, street = 0, roof = 0, weighted = 0;
  if (massings) massings->assign(v.pieces.size(), {});
  for (std::size_t j = 0; j < v.pieces.size(); j++) {
    if (v.built) {
      const double storeys = std::max(1.0, std::round(site.lot_height[j] / site.rules.storey));
      M.floor += v.pieces[j].area() * storeys;
      roof += v.pieces[j].area();
      M.height = std::max(M.height, site.lot_height[j]);
      weighted += v.pieces[j].area() * storeys * v.reach[j];
      ar::Massing m{v.pieces[j].ring, site.lot_height[j], {}, 0};
      const auto f = fr::frontage(m, v.pieces[j], ar::Rules{});
      wall += f.wall_length, street += f.street_length;
      podium_h[j] = site.lot_height[j];
      if (massings) (*massings)[j] = m;
      continue;
    }
    if (v.podium_var[j] < 0) continue;
    const int i = v.lot_of[j];
    const int c = static_cast<int>(d.level[i]);
    const auto m = af::massing_within(v.envelope[j], v.upper_ring[j][c], v.pieces[j].area(), site.rules, static_cast<int>(d.podium[i]), static_cast<int>(d.upper[i]));
    if (massings) (*massings)[j] = m;
    h[v.podium_var[j]] = m.podium_height;
    podium_h[j] = m.podium_height;
    if (!m.upper.empty() && v.upper_var[j][c] >= 0) h[v.upper_var[j][c]] = m.upper_height, top[j] = m.upper_height, chosen[j] = c;
    const double floor = ar::floor_area(m, site.rules.storey);
    M.floor += floor;
    weighted += floor * v.reach[j];
    roof += m.podium_height > 0 ? area(m.podium) : (m.upper.empty() ? 0.0 : area(m.upper));
    M.height = std::max({M.height, m.podium_height, m.upper.empty() ? 0.0 : m.upper_height});
    const auto f = fr::frontage(m, v.pieces[j], site.rules);
    wall += f.wall_length, street += f.street_length;
  }
  M.frontage = street > 0 ? wall / street : 0;
  M.access = M.floor > 0 ? weighted / M.floor : 0;
  M.configuration = v.configuration;
  const std::size_t K = site.suns.size();
  for (auto r : v.ground)
    for (std::size_t k = 0; k < K; k++)
      if (ae::sunlit(v.S, r, k, h)) M.street_sun += site.suns[k].weight * v.ground_cell;
  for (auto r : v.neighbour)
    if (ae::exposed(v.C, v.S.receivers, r, h))
      for (std::size_t k = 0; k < K; k++)
        if (ae::sunlit(v.S, r, k, h)) M.facade_sun += site.suns[k].weight * v.neighbour_cell;
  std::vector<std::pair<std::size_t, double>> walls;
  for (const auto& w : v.walls) {
    if (w.piece < 0) continue;
    const bool stands = v.built ? w.z < podium_h[w.piece]
                                : (w.part < 0 ? w.z < podium_h[w.piece] : (w.part == chosen[w.piece] && w.z >= podium_h[w.piece] && w.z < top[w.piece]));
    if (stands) walls.push_back({w.receiver, w.area});
  }
  M.need = ae::heat_need(v.S, v.C, h, walls, roof, site.day);
  M.heat = M.floor > 0 ? M.need / M.floor : std::numeric_limits<double>::infinity();
  if (heights_out) *heights_out = h;
  return M;
}

// The floor within a walk of each piece's door: the midpoint of its longest street edge, joined to the walking network.
inline void reach_of(Site& site, Variant& v, const std::vector<std::vector<double>>& ways, const std::vector<std::pair<cg::Point2, double>>& opportunities) {
  std::vector<ua::Way> w;
  for (const auto& xy : ways) w.push_back({xy, WALK});
  auto n = ua::network_of(w, 1.0);
  ua::stitch_ends(n, 2.0, WALK);
  ua::stitch_to_ways(n, 2.0, WALK);
  const auto ok = ua::joinable(n);
  std::vector<ua::Joined> at;
  for (const auto& [p, f] : opportunities) at.push_back(ua::join(n, p[0], p[1], ok));
  v.reach.assign(v.pieces.size(), 0);
  for (std::size_t j = 0; j < v.pieces.size(); j++) {
    const auto& lot = v.pieces[j];
    double best = -1;
    cg::Point2 door = centroid(lot.ring);
    for (std::size_t k = 0; k < lot.ring.size(); k++) {
      if (k >= lot.street.size() || !lot.street[k]) continue;
      const auto a = lot.ring[k], b = lot.ring[(k + 1) % lot.ring.size()];
      const double L = std::hypot(b[0] - a[0], b[1] - a[1]);
      if (L > best) best = L, door = {(a[0] + b[0]) / 2, (a[1] + b[1]) / 2};
    }
    const auto from = ua::join(n, door[0], door[1], ok);
    const double start = from.gap / WALK;
    if (start > WALK_LIMIT) continue;
    const auto sp = morphe::combinatorics::dijkstra(n.arcs, {{from.point, start}}, WALK_LIMIT);
    for (std::size_t o = 0; o < opportunities.size(); o++) {
      const double t = sp.distance[at[o].point] + at[o].gap / WALK;
      if (t <= WALK_LIMIT) v.reach[j] += opportunities[o].second;
    }
  }
}

// The mean integration (NAIN, angular, radius 800 m) of the streets within 300 m of the block; and the passage's choice (NACH).
inline void configure(Site& site, Variant& v, const std::vector<std::vector<double>>& ways, const std::array<cg::Point2, 2>* passage) {
  const auto g = urban::syntax::street_graph(ways);
  const auto map = urban::syntax::segment_map(g);
  if (map.size() < 2) return;
  const auto c = urban::syntax::configuration(map, urban::syntax::Cost::angular, SYNTAX_RADIUS, threads());
  const auto mid = centroid(site.block);
  double sum = 0;
  int count = 0;
  for (std::size_t s = 0; s < map.size(); s++) {
    const auto m = (map.a[s] + map.b[s]) / 2.0;
    if (std::hypot(m.real() - mid[0], m.imag() - mid[1]) > SCORE_RADIUS) continue;
    sum += c.integration[s], count++;
  }
  v.configuration = count ? sum / count : 0;
  if (passage) {
    const urban::syntax::Point p{((*passage)[0][0] + (*passage)[1][0]) / 2, ((*passage)[0][1] + (*passage)[1][1]) / 2};
    const auto n = urban::syntax::nearest_segment(map, p);
    if (n.segment < map.size() && n.distance < 5) v.passage_nach = c.nach[n.segment];
  }
}

// ---- the search ----------------------------------------------------------------------------------------------------------------

struct Operators {
  const Site* site;
  hv::IntegerBox podium, upper, level;
  std::size_t n;
  std::int64_t passages;
  explicit Operators(const Site& s)
      : site(&s),
        podium(hv::IntegerBox::cube(s.lots.size(), 0, s.podium_max)),
        upper(hv::IntegerBox::cube(s.lots.size(), 0, s.upper_max)),
        level(hv::IntegerBox::cube(s.lots.size(), 0, 3)),
        n(s.lots.size()),
        passages(static_cast<std::int64_t>(s.passages.size())) {}
  Design sample(calx::random::Stream& s) const {
    Design d;
    d.podium = hv::uniform_integers(podium, s), d.upper = hv::uniform_integers(upper, s), d.level = hv::uniform_integers(level, s);
    d.passage = static_cast<std::int64_t>(std::floor(s.uniform() * static_cast<double>(passages + 1)));
    return d;
  }
  // Storeys crept at two lots in n on average, by at most a quarter of their range; a level redrawn at one in n; the passage at 1 in 10.
  Design mutate(const Design& x, calx::random::Stream& s) const {
    Design y;
    const double rate = 2.0 / static_cast<double>(n);
    y.podium = hv::creep_mutation(x.podium, podium, std::max<std::int64_t>(1, site->podium_max / 4), rate, s);
    y.upper = hv::creep_mutation(x.upper, upper, std::max<std::int64_t>(1, site->upper_max / 4), rate, s);
    y.level = hv::reset_mutation(x.level, level, 1.0 / static_cast<double>(n), s);
    y.passage = s.uniform() < 0.1 ? static_cast<std::int64_t>(std::floor(s.uniform() * static_cast<double>(passages + 1))) : x.passage;
    return y;
  }
  // A lot's three numbers travel together; lots are in order along the block, so two cuts exchange a stretch of it.
  std::pair<Design, Design> crossover(const Design& a, const Design& b, calx::random::Stream& s) const {
    if (s.uniform() >= 0.9) return {a, b};
    std::vector<std::array<std::int64_t, 3>> za(n), zb(n);
    for (std::size_t q = 0; q < n; q++) {
      const auto i = site->order[q];
      za[q] = {a.podium[i], a.upper[i], a.level[i]}, zb[q] = {b.podium[i], b.upper[i], b.level[i]};
    }
    auto [ca, cb] = hv::two_point_crossover(za, zb, s);
    Design x = a, y = b;
    for (std::size_t q = 0; q < n; q++) {
      const auto i = site->order[q];
      x.podium[i] = ca[q][0], x.upper[i] = ca[q][1], x.level[i] = ca[q][2];
      y.podium[i] = cb[q][0], y.upper[i] = cb[q][1], y.level[i] = cb[q][2];
    }
    if (s.uniform() < 0.5) std::swap(x.passage, y.passage);
    return {x, y};
  }
  mo::Kernels<Design> kernels() const {
    return {[this](calx::random::Stream& s) { return sample(s); },
            [this](const Design& a, const Design& b, calx::random::Stream& s) { return crossover(a, b, s); },
            [this](const Design& x, calx::random::Stream& s) { return mutate(x, s); }};
  }
};

inline const Variant& variant_of(const Site& site, const Design& d) { return site.variants[1 + static_cast<std::size_t>(d.passage)]; }

inline hs::Evaluation evaluate(const Site& site, const Design& d) {
  const auto M = measure(site, variant_of(site, d), d);
  const auto& B = site.as_built;
  const auto ref = [](double v) { return v > 0 ? v : 1.0; };
  hs::Evaluation e;
  e.objectives = {-M.floor / ref(B.floor), -M.street_sun / ref(B.street_sun), -M.facade_sun / ref(B.facade_sun),
                  std::isfinite(M.heat) ? M.heat / ref(B.heat) : 1e6, -M.configuration / ref(B.configuration), -M.access / ref(B.access)};
  e.constraints = {site.min_frontage - M.frontage, site.least_floor * B.floor - M.floor};
  return e;
}

inline std::vector<Individual> feasible_front(const std::vector<Individual>& pop) {
  std::vector<std::size_t> ok;
  for (std::size_t i = 0; i < pop.size(); i++)
    if (pop[i].evaluation.feasible()) ok.push_back(i);
  std::vector<Individual> out;
  for (auto i : ok) {
    bool dominated = false;
    for (auto j : ok)
      if (j != i && mo::dominates(pop[j].evaluation.objectives, pop[i].evaluation.objectives)) { dominated = true; break; }
    bool twin = false;
    for (const auto& x : out) twin = twin || x.evaluation.objectives == pop[i].evaluation.objectives;
    if (!dominated && !twin) out.push_back(pop[i]);
  }
  return out;
}

inline Json failing_json(const std::map<std::string, double>& failing) {
  Json o = Json::object();
  for (const auto& [why, n] : failing) o.set(why, Json(n));
  return o;
}

inline Json measures_json(const Measures& M, double lot_area) {
  return Json::object({{"floor", fixed(M.floor, 1)}, {"plotRatio", fixed(lot_area > 0 ? M.floor / lot_area : 0, 100)}, {"streetSun", fixed(M.street_sun, 1)},
                       {"facadeSun", fixed(M.facade_sun, 1)}, {"heat", fixed(M.heat, 100)}, {"configuration", fixed(M.configuration, 10000)},
                       {"access", fixed(M.access, 1)}, {"frontage", fixed(M.frontage, 1000)}, {"height", fixed(M.height, 10)}});
}

// ---- the operations ------------------------------------------------------------------------------------------------------------

// What the operations share: the site and front of the last design, which sunmap reads.
class Designer {
 public:
  Json design(const Json& request);
  Json sunmap(const Json& request) const;
  const Site* last() const { return last_.get(); }

 private:
  std::unique_ptr<Site> last_;
};

inline Json Designer::design(const Json& r) {
  const auto t0 = std::chrono::steady_clock::now();
  auto site = std::make_unique<Site>();
  Site& S = *site;
  const cg::Point2 at{r["at"][0].as_number(), r["at"][1].as_number()};
  const auto streets = polylines(r["streets"]), ways = polylines(r["ways"]);
  if (streets.empty()) throw std::runtime_error("no streets drawn here yet: try again once the ways have come");
  S.block = ccw(block_about(streets, at));
  // The rules, declared by the caller.
  const Json& q = r["rules"];
  S.rules.max_height = number_or(q, "max_height", 60);
  S.rules.plot_ratio = number_or(q, "plot_ratio", 6);
  S.rules.street_setback = number_or(q, "street_setback", 0);
  S.rules.side_setback = number_or(q, "side_setback", 0);
  S.rules.street_wall = number_or(q, "street_wall", 20);
  S.rules.upper_setback = number_or(q, "upper_setback", 5);
  S.rules.storey = number_or(q, "storey", 3.2);
  S.min_frontage = number_or(q, "min_frontage", 0);
  S.least_floor = number_or(r, "least_floor", 0.5);
  S.podium_max = static_cast<int>(std::floor(std::min({S.rules.street_wall, S.rules.max_height, 40.0}) / S.rules.storey + 1e-9));
  S.upper_max = static_cast<int>(std::floor(std::min(S.rules.max_height, 200.0) / S.rules.storey + 1e-9));
  // The day: the Sun's instants with the air at each, and the night.
  for (std::size_t k = 0; k < r["suns"].size(); k++) {
    const Json& s = r["suns"][k];
    if (s[0].as_number() <= 0) continue;
    S.suns.push_back({s[0].as_number(), s[1].as_number(), s[2].as_number()});
    S.day.air.push_back(s.size() > 3 ? s[3].as_number() : 10.0);
  }
  if (S.suns.empty()) throw std::runtime_error("the Sun never rises on the day given");
  S.day.night_air = r.has("night") ? number_or(r["night"], "air", 5) : 5;
  S.day.night_hours = r.has("night") ? number_or(r["night"], "hours", 24 - static_cast<double>(S.suns.size())) : 24 - static_cast<double>(S.suns.size());
  // The buildings: those of the block (of 30 m² or more) its lots, the rest about it the context.
  std::vector<Building> all;
  for (std::size_t k = 0; k < r["buildings"].size(); k++) {
    const Json& b = r["buildings"][k];
    Building B;
    B.height = std::max(3.0, b[0].as_number());
    for (std::size_t i = 1; i + 1 < b.size(); i += 2) B.ring.push_back({b[i].as_number(), b[i + 1].as_number()});
    if (B.ring.size() >= 3 && area(B.ring) > 1) B.ring = ccw(B.ring), all.push_back(std::move(B));
  }
  std::vector<Building> in;
  for (auto& b : all) {
    if (cg::inside_ring(S.block, centroid(b.ring))) {
      if (area(b.ring) >= 30) in.push_back(b);
    } else S.context.push_back(b);
  }
  if (in.size() > 150) throw std::runtime_error("the block holds " + std::to_string(in.size()) + " buildings: pick a smaller block");
  if (in.empty()) {
    // An empty block is one lot, half a street in from the streets' centrelines.
    ar::Lot lot;
    lot.ring = cg::inset(S.block, std::vector<double>(S.block.size(), 7.0));
    if (lot.ring.size() < 3) throw std::runtime_error("the block is too small to build on");
    lot.street.assign(lot.ring.size(), 1);
    S.lots.push_back(lot), S.lot_height.push_back(0);
  }
  for (const auto& b : in) {
    ar::Lot lot{b.ring, std::vector<char>(b.ring.size(), 0)};
    S.lots.push_back(lot), S.lot_height.push_back(b.height);
  }
  // Each lot's edges that face a street: the ray out from its middle reaches the block's edge within 40 m before any other lot.
  for (std::size_t i = 0; i < S.lots.size(); i++) {
    auto& L = S.lots[i];
    for (std::size_t k = 0; k < L.ring.size(); k++) {
      const auto a = L.ring[k], b = L.ring[(k + 1) % L.ring.size()];
      const double len = std::hypot(b[0] - a[0], b[1] - a[1]);
      if (len < 0.5) continue;
      const cg::Point2 u{(b[1] - a[1]) / len, -(b[0] - a[0]) / len};  // outward for a counter-clockwise ring
      const cg::Point2 m{(a[0] + b[0]) / 2 + 0.05 * u[0], (a[1] + b[1]) / 2 + 0.05 * u[1]};
      const double tb = cg::ray_entry(S.block, m, u);
      if (!(tb <= 40)) continue;
      bool blocked = false;
      for (std::size_t j = 0; j < S.lots.size() && !blocked; j++) blocked = j != i && cg::ray_entry(S.lots[j].ring, m, u) < tb;
      L.street[k] = !blocked;
    }
  }
  // The lots in order along the block's long axis.
  {
    const auto c = centroid(S.block);
    const auto P = passages_of(S.block);
    cg::Point2 axis{1, 0};
    if (P.size() == 2) {
      const double L = std::hypot(P[1][1][0] - P[1][0][0], P[1][1][1] - P[1][0][1]);
      if (L > 0) axis = {(P[1][1][0] - P[1][0][0]) / L, (P[1][1][1] - P[1][0][1]) / L};
    }
    S.order.resize(S.lots.size());
    std::iota(S.order.begin(), S.order.end(), std::size_t{0});
    std::vector<double> key(S.lots.size());
    for (std::size_t i = 0; i < S.lots.size(); i++) {
      const auto m = centroid(S.lots[i].ring);
      key[i] = (m[0] - c[0]) * axis[0] + (m[1] - c[1]) * axis[1];
    }
    std::sort(S.order.begin(), S.order.end(), [&](std::size_t a, std::size_t b) { return key[a] < key[b]; });
    S.passages = P;
  }
  // The floor about the block, as opportunities within a walk: each building's footprint by its storeys.
  std::vector<std::pair<cg::Point2, double>> opportunities;
  for (const auto& b : S.context) opportunities.push_back({centroid(b.ring), area(b.ring) * std::max(1.0, std::round(b.height / S.rules.storey))});
  // The variants: as built, without a passage, and with each passage.
  const std::size_t V = 2 + S.passages.size();
  S.variants.resize(V);
  for (std::size_t vi = 0; vi < V; vi++) {
    Variant& v = S.variants[vi];
    v.built = vi == 0;
    v.passage = vi >= 2 ? static_cast<int>(vi) - 2 : -1;
    v.name = vi == 0 ? "as built" : vi == 1 ? "no passage" : (vi == 2 ? "a passage across the block" : "a passage along the block");
    std::vector<ae::Prism> parts;
    std::vector<std::array<int, 2>> owners;
    if (v.built) {
      for (std::size_t i = 0; i < S.lots.size(); i++) {
        v.pieces.push_back(S.lots[i]), v.lot_of.push_back(static_cast<int>(i));
        v.podium_var.push_back(-1), v.upper_var.push_back({-1, -1, -1, -1});
        v.envelope.push_back({}), v.ground_env.push_back(S.lots[i].ring), v.upper_ring.push_back({});
        parts.push_back({S.lots[i].ring, S.lot_height[i], -1});
        owners.push_back({static_cast<int>(i), -1});
      }
    } else {
      for (std::size_t i = 0; i < S.lots.size(); i++) {
        std::vector<ar::Lot> ps = v.passage >= 0 ? ar::split(S.lots[i], S.passages[v.passage][0], S.passages[v.passage][1], PASSAGE, 4.0) : std::vector<ar::Lot>{S.lots[i]};
        for (auto& p : ps) {
          const auto e = ar::envelope(p, S.rules);
          v.pieces.push_back(p), v.lot_of.push_back(static_cast<int>(i));
          v.envelope.push_back(e), v.ground_env.push_back(e.ground);
          std::array<Ring, 4> ups{};
          std::array<int, 4> uv{-1, -1, -1, -1};
          int pv = -1;
          if (e.ground.size() >= 3) {
            pv = static_cast<int>(v.variables++);
            parts.push_back({e.ground, 0, pv});
            owners.push_back({static_cast<int>(v.pieces.size()) - 1, -1});
            if (e.upper.size() >= 3)
              for (int c = 0; c < 4; c++) {
                ups[c] = af::upper_part(e.upper, LEVELS[c]);
                uv[c] = static_cast<int>(v.variables++);
                parts.push_back({ups[c], 0, uv[c]});
                owners.push_back({static_cast<int>(v.pieces.size()) - 1, c});
              }
          }
          v.podium_var.push_back(pv), v.upper_var.push_back(uv), v.upper_ring.push_back(ups);
        }
      }
    }
    receive(S, v, parts, owners);
    const auto w = v.passage >= 0 ? with_passage(ways, S.passages[v.passage]) : ways;
    reach_of(S, v, w, opportunities);
    if (vi <= 1 || v.passage >= 0) configure(S, v, w, v.passage >= 0 ? &S.passages[v.passage] : nullptr);
  }
  S.variants[0].configuration = S.variants[1].configuration;
  S.as_built = measure(S, S.variants[0], Design{});
  const auto t1 = std::chrono::steady_clock::now();
  // The search: NSGA-III on six objectives, each relative to the block as built.
  const Operators ops(S);
  const auto references = mo::das_dennis(6, 4);
  const std::size_t mu = (references.size() + 3) / 4 * 4;
  const std::size_t generations = static_cast<std::size_t>(std::clamp(number_or(r, "generations", 60), 2.0, 400.0));
  const auto seed = static_cast<std::uint64_t>(number_or(r, "seed", 1));
  mo::Nsga3<Design> alg(ops.kernels(), references, mu, hs::Draws(seed));
  calx::scheduler::ThreadPool pool(threads());
  const auto f = [&S](const Design& d) { return evaluate(S, d); };
  const auto o = hs::run(alg, f, hs::Budget{static_cast<double>(mu * generations)}, hs::Pool(pool));
  S.front = feasible_front(alg.population());
  // Every design of the front complies with the rules, as the domain's massing promises; checked.
  std::size_t complying = 0, pieces = 0;
  std::map<std::string, double> failing;
  for (const auto& x : S.front) {
    const auto& v = variant_of(S, x.genotype);
    std::vector<ar::Massing> ms;
    measure(S, v, x.genotype, nullptr, &ms);
    for (std::size_t j = 0; j < v.pieces.size(); j++) {
      if (v.podium_var[j] < 0) continue;
      pieces++;
      const auto c = ar::complies(ms[j], v.pieces[j], S.rules);
      if (c.ok || (ms[j].podium_height <= 0 && ms[j].upper.empty())) complying++;
      else for (const auto& why : c.reasons) failing[why]++;
    }
  }
  const auto t2 = std::chrono::steady_clock::now();
  // The answer.
  double lot_area = 0;
  for (const auto& L : S.lots) lot_area += L.area();
  Json lots = Json::array();
  for (std::size_t i = 0; i < S.lots.size(); i++) {
    Json st = Json::array();
    for (char c : S.lots[i].street) st.push(Json(static_cast<double>(c)));
    lots.push(Json::object({{"ring", ring_json(S.lots[i].ring)}, {"street", st}, {"height", fixed(S.lot_height[i], 10)}}));
  }
  Json passages = Json::array();
  for (const auto& p : S.passages) passages.push(Json::array({fixed(p[0][0]), fixed(p[0][1]), fixed(p[1][0]), fixed(p[1][1])}));
  Json variants = Json::array();
  for (std::size_t vi = 1; vi < S.variants.size(); vi++) {
    const auto& v = S.variants[vi];
    Json ps = Json::array();
    for (std::size_t j = 0; j < v.pieces.size(); j++) {
      Json ups = Json::array();
      for (int c = 0; c < 4; c++) ups.push(v.upper_ring[j][c].empty() ? Json() : ring_json(v.upper_ring[j][c]));
      ps.push(Json::object({{"lot", Json(static_cast<double>(v.lot_of[j]))}, {"ground", v.ground_env[j].size() >= 3 ? ring_json(v.ground_env[j]) : Json()}, {"upper", ups}}));
    }
    variants.push(Json::object({{"name", v.name}, {"pieces", ps}, {"configuration", fixed(v.configuration, 10000)}, {"passageChoice", fixed(v.passage_nach, 10000)}}));
  }
  Json designs = Json::array();
  for (const auto& x : S.front) {
    const auto& v = variant_of(S, x.genotype);
    std::vector<ar::Massing> ms;
    const auto M = measure(S, v, x.genotype, nullptr, &ms);
    Json parts = Json::array();
    for (std::size_t j = 0; j < v.pieces.size(); j++) {
      const int i = v.lot_of[j];
      const bool up = !ms[j].upper.empty();
      parts.push(Json::array({fixed(ms[j].podium_height, 10), Json(up ? static_cast<double>(x.genotype.level[i]) : -1.0), fixed(up ? ms[j].upper_height : 0, 10)}));
    }
    designs.push(Json::object({{"variant", Json(static_cast<double>(x.genotype.passage))}, {"measures", measures_json(M, lot_area)}, {"parts", parts}}));
  }
  std::size_t receivers = 0;
  for (const auto& v : S.variants) receivers += v.S.receivers.size();
  const double prep = std::chrono::duration<double>(t1 - t0).count(), search = std::chrono::duration<double>(t2 - t1).count();
  Json out = Json::object({{"block", ring_json(S.block)}, {"lots", lots}, {"passages", passages}, {"variants", variants},
                           {"asBuilt", measures_json(S.as_built, lot_area)}, {"designs", designs}, {"lotArea", fixed(lot_area, 1)},
                           {"leastFloor", fixed(S.least_floor * S.as_built.floor, 1)}, {"plotRatioFloor", fixed(std::isfinite(S.rules.plot_ratio) ? S.rules.plot_ratio * lot_area : -1, 1)},
                           {"minFrontage", Json(S.min_frontage)},
                           {"evaluations", Json(o.evaluations)}, {"generations", Json(static_cast<double>(generations))}, {"population", Json(static_cast<double>(mu))},
                           {"complying", Json(static_cast<double>(complying))}, {"pieces", Json(static_cast<double>(pieces))}, {"failing", failing_json(failing)},
                           {"receivers", Json(static_cast<double>(receivers))}, {"context", Json(static_cast<double>(S.context.size()))},
                           {"seconds", Json::object({{"preparing", fixed(prep, 100)}, {"searching", fixed(search, 100)}})}, {"threads", Json(static_cast<double>(threads()))}});
  last_ = std::move(site);
  return out;
}

inline Json Designer::sunmap(const Json& r) const {
  if (!last_) throw std::runtime_error("no design yet: design a block first");
  const Site& S = *last_;
  const int k = static_cast<int>(number_or(r, "design", -1));
  const Variant* v = &S.variants[0];
  std::vector<double> h;
  if (k >= 0) {
    if (static_cast<std::size_t>(k) >= S.front.size()) throw std::runtime_error("no such design");
    v = &variant_of(S, S.front[k].genotype);
    measure(S, *v, S.front[k].genotype, &h);
  }
  Json pts = Json::array();
  for (auto rr : v->ground) {
    double hours = 0;
    for (std::size_t q = 0; q < S.suns.size(); q++)
      if (ae::sunlit(v->S, rr, q, h)) hours += S.suns[q].weight;
    const auto& p = v->S.receivers[rr];
    pts.push(Json::array({fixed(p.x), fixed(p.y), fixed(hours, 100)}));
  }
  double total = 0;
  for (const auto& s : S.suns) total += s.weight;
  return Json::object({{"points", pts}, {"spacing", Json(SPACING)}, {"hours", fixed(total, 100)}});
}

}  // namespace science::design
