// A street read in one call (ADR 0098): the urban-science domain's section along a way, composed for any work.
//
// A work that shows a street (the Design Studio first) holds its centreline, the buildings about it and, where it is being
// designed, the strips it is divided into and what a scheme asks of them. This header reads those, calls the domain, and answers
// in JSON. It names no place and no authority: the buildings are whatever the work read of a place (or drew), and a limit is one
// only when the request states it.
//
// THE REQUEST (lengths in metres; x east, y north; angles in degrees):
//   {"op": "street",
//    "street": {"line": [[x, y], …],                                the centreline, in the order it is walked
//               "strips": [{"kind", "width"}],                      from the left: footpath, frontage, shared, cycle, verge, median,
//                                                                   bus, tram, parking, lane
//               "scheme": {"name", "width": {kind: limit}, "enclosure": limit, "people": limit, "sun": limit},
//                                                                   a limit is {"least", "most", "clause"}
//               "need": hours, "step": metres, "reach": metres},    hours of sun to keep (2); between stations (5); to a face (40)
//    "around": {"buildings": [{"ring": [[x, y], …], "height"}],     prisms on flat ground
//               "suns": [[altitude, azimuth, hours], …]}}           the Sun at the instants of a day; azimuth from north through east
//
// THE ANSWER: the street's length; its stations (where each stands, its left, the face on each side with its distance, height and
// building, the width, the enclosure, the sky, and each footpath's point and hours of sun); each side's line, the shares of its
// length built and aligned, its mean height and sun; the least and median widths, the enclosure and the sky of the whole; the
// strips set out with their groups' shares and whether they fit; the findings of the scheme one by one; and of each building that
// stands on the street, its height, the height it may rise to before a footpath loses the hours asked, and the sun it takes from
// the footpaths as it stands.
// A number that has no value (an open side's distance, a limit that is not reached) is null.

#pragma once

#include "science/json.hpp"

#include <urban/section/section.hpp>

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::street {

namespace us = urban::section;
namespace ae = architecture::environment;
namespace ar = architecture::rules;

namespace detail {

inline double number(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }
inline Json maybe(std::size_t i) { return i == us::none ? Json() : Json(i); }
inline Json point(const us::Point& p) { return Json::array({p[0], p[1]}); }

inline us::Point read_point(const Json& j, const char* what) {
  if (!j.is_array() || j.size() < 2 || !j[0].is_number() || !j[1].is_number()) throw std::runtime_error(std::string(what) + " is [x, y]");
  return {j[0].as_number(), j[1].as_number()};
}

inline ar::Limit limit(const Json& j) {
  ar::Limit l;
  if (!j.is_object()) return l;
  if (j["least"].is_number()) l.least = j["least"].as_number();
  if (j["most"].is_number()) l.most = j["most"].as_number();
  l.clause = j["clause"].text();
  return l;
}

}  // namespace detail

inline Json read(const Json& street, const Json& around) {
  std::vector<us::Point> line;
  for (const auto& p : street["line"].is_array() ? street["line"].as_array() : Json::Array{}) line.push_back(detail::read_point(p, "a point of the centreline"));
  std::vector<us::Strip> strips;
  for (const auto& s : street["strips"].is_array() ? street["strips"].as_array() : Json::Array{}) strips.push_back({s["kind"].text(), detail::number(s["width"], 0)});
  us::Scheme scheme;
  const Json& js = street["scheme"];
  scheme.name = js["name"].text();
  if (js["width"].is_object())
    for (const auto& [kind, l] : js["width"].as_object()) scheme.width[kind] = detail::limit(l);
  scheme.enclosure = detail::limit(js["enclosure"]), scheme.people = detail::limit(js["people"]), scheme.sun = detail::limit(js["sun"]);
  std::vector<us::Building> buildings;
  for (const auto& b : around["buildings"].is_array() ? around["buildings"].as_array() : Json::Array{}) {
    us::Building one;
    for (const auto& p : b["ring"].is_array() ? b["ring"].as_array() : Json::Array{}) one.ring.push_back(detail::read_point(p, "a corner of a building"));
    one.height = detail::number(b["height"], 0);
    buildings.push_back(std::move(one));   // kept in the request's order, so that an index names the same building in the answer
  }
  std::vector<ae::Sun> suns;
  for (const auto& s : around["suns"].is_array() ? around["suns"].as_array() : Json::Array{}) {
    const double altitude = detail::number(s[0], 0), azimuth = detail::number(s[1], 0), hours = detail::number(s[2], 0);
    if (altitude > 0 && hours > 0) suns.push_back({altitude * std::numbers::pi / 180, azimuth * std::numbers::pi / 180, hours});
  }
  const double need = detail::number(street["need"], 2.0), step = detail::number(street["step"], 5.0), reach = detail::number(street["reach"], 40.0);
  if (!(need >= 0 && step >= 1 && step <= 50 && reach >= 5 && reach <= 200)) throw std::runtime_error("a street is read with hours not negative, a step from 1 to 50 m and a reach from 5 to 200 m");
  const us::Reading R = us::read(line, buildings, strips, suns, scheme, need, step, reach);

  Json out = Json::object({{"length", R.length}, {"lit", R.lit}, {"need", need}, {"step", step}, {"width_least", detail::finite(R.width_least)},
                           {"width_median", detail::finite(R.width_median)}, {"enclosure", R.enclosure}, {"sky", R.sky}, {"total", R.total}, {"fits", R.fits}});
  Json stations = Json::array();
  for (const auto& st : R.stations) {
    Json faces = Json::array();
    for (int s = 0; s < 2; s++)
      faces.push(Json::object({{"distance", detail::finite(st.face[s].distance)}, {"height", st.face[s].height}, {"building", detail::maybe(st.face[s].building)}}));
    stations.push(Json::object({{"at", st.at}, {"point", detail::point(st.point)}, {"left", detail::point(st.left)}, {"faces", faces}, {"width", detail::finite(st.width)},
                                {"enclosure", st.enclosure}, {"sky", st.sky}, {"sun", Json::array({st.sun[0], st.sun[1]})},
                                {"foot", Json::array({detail::point(st.foot[0]), detail::point(st.foot[1])})}}));
  }
  out.set("stations", stations);
  Json sides = Json::array();
  for (int s = 0; s < 2; s++)
    sides.push(Json::object({{"line", detail::finite(R.side[s].line)}, {"built", R.side[s].built}, {"aligned", R.side[s].aligned}, {"height", R.side[s].height},
                             {"sun", R.side[s].sun}, {"sunny", R.side[s].sunny}}));
  out.set("sides", sides);
  Json strips_out = Json::array();
  static const char* groups[5] = {"people", "cycles", "green", "transit", "vehicles"};
  for (const auto& s : R.strips) strips_out.push(Json::object({{"kind", s.kind}, {"from", s.from}, {"to", s.to}, {"group", groups[s.group]}}));
  out.set("strips", strips_out);
  out.set("shares", Json::object({{"people", R.share[0]}, {"cycles", R.share[1]}, {"green", R.share[2]}, {"transit", R.share[3]}, {"vehicles", R.share[4]}}));
  Json findings = Json::array();
  for (const auto& f : R.findings)
    findings.push(Json::object({{"rule", f.rule}, {"title", f.title}, {"clause", f.clause}, {"subject", f.subject}, {"least", f.least}, {"most", f.most}, {"actual", f.actual},
                                {"unit", f.unit}, {"ok", f.ok}, {"note", f.note}}));
  out.set("findings", findings);
  Json rises = Json::array();
  for (const auto& r : R.rises) rises.push(Json::object({{"building", r.building}, {"height", r.height}, {"limit", detail::finite(r.limit)}, {"reached", r.reached}, {"takes", r.takes}}));
  out.set("rises", rises);
  return out;
}

inline Json answer(const Json& request) { return read(request["street"], request["around"]); }

}  // namespace science::street
