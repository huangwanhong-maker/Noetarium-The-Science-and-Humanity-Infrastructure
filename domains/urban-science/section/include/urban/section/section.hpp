// A street's section, along its length (ADR 0098): what stands on either side of a way, the width between, how enclosed it is and
// how much sky it sees, the sun on its footpaths, the strips it is divided into, and how high each building beside it may rise
// before it takes the footpaths' sun.
//
// A STREET here is a centreline (a polyline on the ground, metres east and north) with the buildings about it as prisms on flat
// ground (a footprint ring and a height). Nothing is said of who owns what: the width of a street is what is measured between the
// faces that stand on it.
//
// STATIONS stand every `step` metres along the centreline. At each, a ray is sent to the left and to the right, square to the
// centreline: the first face it meets within `reach` is that side's FACE, at a distance d and of a height H (a building the
// centreline passes under is not a face; of two met at one distance, the higher). With faces on both sides the WIDTH is W = d_l + d_r, the ENCLOSURE is the mean height
// over the width, (H_l + H_r) / 2 / W, and the SKY the centreline sees from the ground is that of a canyon of this section running
// on without end: ψ = (cos β_l + cos β_r) / 2, tan β = H / d (a point's view factor to an endless wall of angular height β is
// (1 − cos β) / 2; Oke 1981, Johnson and Watson 1984). A side with no face is open: it hides no sky and encloses nothing.
//
// EACH SIDE has a LINE, the median of its faces' distances; the share of the street's length that is BUILT (a face within reach)
// and the share that is ALIGNED (a face within 2 m of the line): the continuity of the street wall.
//
// THE STRIPS divide the street from left to right, looking along the centreline: footpaths, verges, cycle tracks, parking, lanes,
// a median. They are centred on the centreline. Their widths sum to a total that FITS when it is no more than the least width
// between faces; each strip belongs to a group (people, cycles, green, transit, vehicles), and the groups' shares of the total
// are given. A SCHEME states limits: on a strip's width by its kind, on the enclosure, on the share for people, on the hours of
// sun on a footpath. Nothing here is any authority's until a scheme states it; a limit not stated is not found.
//
// SUN ON THE FOOTPATHS (architecture's sun.hpp). On each side, at each station, a point on the ground: the middle of that side's
// outermost footpath where the strips have one, else one metre out from the face (or, with no face, three metres from the
// centreline). Its hours of sun over the instants given are found against every building; each side's mean, and the share of its
// length with at least `need` hours.
//
// HOW HIGH EACH MAY RISE (architecture's solar.hpp): every building is a variable; its limit is the greatest height at which every
// footpath point that has `need` hours keeps them, the others as they stand. One building at a time: Knowles's solar envelope
// for the footpaths. And what it TAKES as it stands: the sun the footpaths' points would have were it gone, as a mean over them.
//
// Left out: the ground's relief, trees and awnings, the sky's diffuse light, arcades, a way that bends between stations.

#pragma once

#include <architecture/environment/solar.hpp>
#include <architecture/environment/sun.hpp>
#include <architecture/rules/finding.hpp>
#include <physis/computational_geometry/planar.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace urban::section {

namespace cg = physis::computational_geometry;
namespace ae = architecture::environment;
namespace ar = architecture::rules;
using Point = cg::Point2;
using Ring = std::vector<Point>;
inline constexpr std::size_t none = static_cast<std::size_t>(-1);
inline constexpr double inf = std::numeric_limits<double>::infinity();

struct Building {
  Ring ring;
  double height = 0;
};

struct Strip {
  std::string kind;   // footpath, frontage, shared, cycle, verge, median, bus, tram, parking, lane
  double width = 0;
};

// The group a kind of strip belongs to: 0 people, 1 cycles, 2 green, 3 transit, 4 vehicles.
inline int group_of(const std::string& kind) {
  if (kind == "footpath" || kind == "frontage" || kind == "shared") return 0;
  if (kind == "cycle") return 1;
  if (kind == "verge" || kind == "median") return 2;
  if (kind == "bus" || kind == "tram") return 3;
  if (kind == "parking" || kind == "lane") return 4;
  throw std::invalid_argument("section: a strip is a footpath, frontage, shared, cycle, verge, median, bus, tram, parking or lane, not " + kind);
}

struct Scheme {
  std::string name;
  std::map<std::string, ar::Limit> width;   // of a strip, by its kind (m)
  ar::Limit enclosure;                      // the street's mean height over width
  ar::Limit people;                         // the share of the strips' width for people on foot
  ar::Limit sun;                            // hours of sun on a footpath, each side's mean
};

struct Face {
  double distance = inf, height = 0;
  std::size_t building = none;
};

struct Station {
  double at = 0;            // metres along the centreline
  Point point{}, left{};    // on the centreline; the unit vector to its left
  std::array<Face, 2> face; // left, right
  double width = inf, enclosure = 0, sky = 1;
  std::array<double, 2> sun{0, 0};   // hours on each side's footpath point
  std::array<Point, 2> foot{};       // those points
};

struct Side {
  double line = inf;        // the median distance of its faces from the centreline
  double built = 0, aligned = 0;
  double height = 0;        // the mean of its faces' heights, over the stations that have one
  double sun = 0, sunny = 0;   // the mean hours on its footpath; the share of stations with at least `need`
};

struct Placed {
  std::string kind;
  double from = 0, to = 0;  // offsets from the centreline, negative to the left
  int group = 0;
};

struct Rise {
  std::size_t building = none;
  double height = 0, limit = inf;
  std::size_t reached = 0;
  double takes = 0;         // the hours of sun it takes from the footpaths as it stands, as a mean over their points
};

struct Reading {
  double length = 0;
  std::vector<Station> stations;
  std::array<Side, 2> side;
  double width_least = inf, width_median = inf, enclosure = 0, sky = 1;
  std::vector<Placed> strips;
  double total = 0;
  std::array<double, 5> share{0, 0, 0, 0, 0};
  bool fits = true;
  std::vector<Rise> rises;                  // the buildings that stand on the street, each with the height it may rise to
  std::vector<ar::Finding> findings;
  bool lit = false;
};

namespace detail {

inline double median(std::vector<double> v) {
  if (v.empty()) return inf;
  std::sort(v.begin(), v.end());
  const std::size_t n = v.size();
  return n % 2 ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2;
}

}  // namespace detail

// The strips set out about the centreline, from the left.
inline std::vector<Placed> place(const std::vector<Strip>& strips) {
  double total = 0;
  for (const Strip& s : strips) {
    if (!(s.width > 0)) throw std::invalid_argument("section: a strip has a width");
    total += s.width;
  }
  std::vector<Placed> out;
  double at = -total / 2;
  for (const Strip& s : strips) {
    out.push_back({s.kind, at, at + s.width, group_of(s.kind)});
    at += s.width;
  }
  return out;
}

inline Reading read(const std::vector<Point>& centreline, const std::vector<Building>& buildings, const std::vector<Strip>& strips = {}, const std::vector<ae::Sun>& suns = {},
                    const Scheme& scheme = {}, double need = 2.0, double step = 5.0, double reach = 40.0) {
  if (centreline.size() < 2) throw std::invalid_argument("section: a street's centreline has two points at least");
  if (!(step > 0 && reach > 0)) throw std::invalid_argument("section: a step and a reach, positive");
  Reading R;
  R.length = cg::polyline_length(centreline);
  if (!(R.length > 0)) throw std::invalid_argument("section: a street has a length");
  R.strips = place(strips);
  for (const Placed& s : R.strips) R.total += s.to - s.from;
  for (const Placed& s : R.strips) R.share[s.group] += (s.to - s.from) / R.total;

  // Where each side's footpath point stands, from the strips: the middle of the outermost footpath, as an offset to that side.
  std::array<double, 2> path{-1, -1};
  for (const Placed& s : R.strips)
    if (s.kind == "footpath") {
      if (path[0] < 0 && (s.from + s.to) / 2 < 0) path[0] = -(s.from + s.to) / 2;
      if ((s.from + s.to) / 2 > 0) path[1] = (s.from + s.to) / 2;
    }

  // The stations, their faces, widths, enclosure and sky.
  const std::size_t n = std::max<std::size_t>(1, static_cast<std::size_t>(std::floor(R.length / step)));
  std::vector<std::array<double, 4>> box(buildings.size());
  for (std::size_t b = 0; b < buildings.size(); b++) {
    box[b] = {inf, inf, -inf, -inf};
    for (const Point& q : buildings[b].ring) box[b] = {std::min(box[b][0], q[0]), std::min(box[b][1], q[1]), std::max(box[b][2], q[0]), std::max(box[b][3], q[1])};
  }
  for (std::size_t i = 0; i < n; i++) {
    Station st;
    st.at = R.length * (i + 0.5) / n;
    st.point = cg::point_at(centreline, st.at);
    const Point ahead = cg::point_at(centreline, std::min(R.length, st.at + 0.5)), behind = cg::point_at(centreline, std::max(0.0, st.at - 0.5));
    const double dx = ahead[0] - behind[0], dy = ahead[1] - behind[1], L = std::hypot(dx, dy);
    st.left = {-dy / L, dx / L};
    for (int s = 0; s < 2; s++) {
      const Point u = s == 0 ? st.left : Point{-st.left[0], -st.left[1]};
      for (std::size_t b = 0; b < buildings.size(); b++) {
        const auto& bx = box[b];
        if (bx[2] < std::min(st.point[0], st.point[0] + reach * u[0]) || bx[0] > std::max(st.point[0], st.point[0] + reach * u[0]) ||
            bx[3] < std::min(st.point[1], st.point[1] + reach * u[1]) || bx[1] > std::max(st.point[1], st.point[1] + reach * u[1]))
          continue;
        if (!(buildings[b].height > 0) || cg::inside_ring(buildings[b].ring, st.point)) continue;
        const double t = cg::ray_entry(buildings[b].ring, st.point, u);
        if (!std::isfinite(t) || t > reach) continue;
        // Of two met at one distance (a building given storey by storey), the higher is the face.
        const double gap = t - st.face[s].distance;
        if (gap < -1e-6 || (std::fabs(gap) <= 1e-6 && buildings[b].height > st.face[s].height)) st.face[s] = {t, buildings[b].height, b};
      }
    }
    double hidden = 0, heights = 0;
    for (int s = 0; s < 2; s++)
      if (std::isfinite(st.face[s].distance)) {
        hidden += (1 - std::cos(std::atan2(st.face[s].height, std::max(st.face[s].distance, 1e-9)))) / 2;
        heights += st.face[s].height;
      }
    st.sky = 1 - hidden;
    if (std::isfinite(st.face[0].distance) && std::isfinite(st.face[1].distance)) {
      st.width = st.face[0].distance + st.face[1].distance;
      st.enclosure = heights / 2 / st.width;
    }
    for (int s = 0; s < 2; s++) {
      const double sign = s == 0 ? 1 : -1;
      double off = path[s] > 0 ? path[s] : std::isfinite(st.face[s].distance) ? st.face[s].distance - 1.0 : 3.0;
      if (std::isfinite(st.face[s].distance)) off = std::min(off, st.face[s].distance - 0.1);
      off = std::max(off, 0.1);
      st.foot[s] = {st.point[0] + sign * st.left[0] * off, st.point[1] + sign * st.left[1] * off};
    }
    R.stations.push_back(st);
  }

  // The street as a whole, and each side.
  std::vector<double> widths;
  double enclosure = 0, sky = 0;
  std::size_t walled = 0;
  for (const Station& st : R.stations) {
    sky += st.sky;
    if (std::isfinite(st.width)) widths.push_back(st.width), enclosure += st.enclosure, walled++;
  }
  R.sky = sky / R.stations.size();
  if (walled) R.enclosure = enclosure / walled, R.width_least = *std::min_element(widths.begin(), widths.end()), R.width_median = detail::median(widths);
  for (int s = 0; s < 2; s++) {
    std::vector<double> d;
    double height = 0;
    for (const Station& st : R.stations)
      if (std::isfinite(st.face[s].distance)) d.push_back(st.face[s].distance), height += st.face[s].height;
    if (d.empty()) continue;
    Side& side = R.side[s];
    side.line = detail::median(d), side.built = static_cast<double>(d.size()) / R.stations.size(), side.height = height / d.size();
    std::size_t aligned = 0;
    for (const double x : d) aligned += std::fabs(x - side.line) <= 2.0 ? 1 : 0;
    side.aligned = static_cast<double>(aligned) / R.stations.size();
  }
  R.fits = !(R.total > R.width_least + 1e-9);

  // The sun on the footpaths, and how high each building beside them may rise.
  R.lit = !suns.empty();
  if (R.lit) {
    std::vector<ae::Receiver> receivers;
    for (const Station& st : R.stations)
      for (int s = 0; s < 2; s++) receivers.push_back({st.foot[s][0], st.foot[s][1], 0, 0, 0, -1});
    std::vector<ae::Prism> prisms;
    std::vector<double> heights;
    for (std::size_t b = 0; b < buildings.size(); b++) {
      prisms.push_back({buildings[b].ring, buildings[b].height, static_cast<int>(b)});
      heights.push_back(buildings[b].height);
    }
    const ae::Shading S = ae::shading(std::move(receivers), suns, prisms, 400);
    const auto hours = ae::sun_hours(S, heights);
    for (std::size_t i = 0; i < R.stations.size(); i++)
      for (int s = 0; s < 2; s++) {
        R.stations[i].sun[s] = hours[2 * i + s];
        R.side[s].sun += hours[2 * i + s] / R.stations.size();
        R.side[s].sunny += (hours[2 * i + s] >= need - 1e-9 ? 1.0 : 0.0) / R.stations.size();
      }
    const auto limits = ae::solar_limits(S, heights, need);
    std::vector<char> stands(buildings.size(), 0);
    for (const Station& st : R.stations)
      for (int s = 0; s < 2; s++)
        if (st.face[s].building != none) stands[st.face[s].building] = 1;
    for (std::size_t b = 0; b < buildings.size(); b++)
      if (stands[b]) R.rises.push_back({b, buildings[b].height, limits[b].height, limits[b].reached, limits[b].taken / (2.0 * R.stations.size())});
  } else {
    std::vector<char> stands(buildings.size(), 0);
    for (const Station& st : R.stations)
      for (int s = 0; s < 2; s++)
        if (st.face[s].building != none && !stands[st.face[s].building]) stands[st.face[s].building] = 1, R.rises.push_back({st.face[s].building, buildings[st.face[s].building].height, inf, 0, 0.0});
  }

  // What a scheme asks.
  for (std::size_t k = 0; k < R.strips.size(); k++) {
    const auto it = scheme.width.find(R.strips[k].kind);
    if (it == scheme.width.end() || !it->second.stated()) continue;
    R.findings.push_back(ar::found("width." + R.strips[k].kind, "Width of a " + R.strips[k].kind, "strip " + std::to_string(k + 1) + " from the left", it->second, R.strips[k].to - R.strips[k].from, "m"));
  }
  if (!R.strips.empty() && std::isfinite(R.width_least))
    R.findings.push_back(ar::found("fits", "The strips fit between the faces", "the street", ar::Limit::at_most(R.width_least), R.total, "m", "the least width between the faces that stand on it"));
  if (scheme.people.stated() && !R.strips.empty()) R.findings.push_back(ar::found("people", "Share of the width for people on foot", "the street", scheme.people, R.share[0], "share"));
  if (scheme.enclosure.stated() && walled) R.findings.push_back(ar::found("enclosure", "Enclosure: height over width", "the street", scheme.enclosure, R.enclosure, "ratio"));
  if (scheme.sun.stated() && R.lit)
    for (int s = 0; s < 2; s++) R.findings.push_back(ar::found("sun", "Hours of sun on the footpath", s == 0 ? "the left side" : "the right side", scheme.sun, R.side[s].sun, "h"));
  return R;
}

}  // namespace urban::section
