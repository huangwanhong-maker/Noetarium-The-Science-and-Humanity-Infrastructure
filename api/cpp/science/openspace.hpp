// Open space and who can reach it, as one composition any work may call (ADR 0154): urban science's `openspace` on a place's walking
// ways, people and open land read as kinds by the caller. Nothing in it names a city.
//
//   openspace.access  {ways: [[x0, y0, …], …], places: [[x, y, people], …], spaces: [{kind, ring: [x0, y0, …], holes: [[…]]}],
//                     area: m² of the analysis area | box: [x0, y0, x1, y1] (spaces counted only within it), readings: [{name, kinds: [kind], least_area?, metres?, along_network?, source?}],
//                     spacing? (of the walking network, 20 m), edge_spacing? (of a space's boundary, 25 m), cell? (of the raster, 5 m)}
//                     each reading's SDG 11.7.1: the counted spaces, their union's area and its share of the analysis area, the share of
//                     people within reach; and each place's distance and whether it is served under the first reading
//
// Coordinates are metres of a local frame. The kinds are the caller's reading of the land (a park, a plaza, sports grounds, green land).

#pragma once

#include <urban/openspace/openspace.hpp>

#include <science/json.hpp>

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::openspace {

namespace uo = urban::openspace;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline std::vector<double> coords(const Json& a) {
  std::vector<double> v;
  for (const auto& x : a.as_array()) v.push_back(x.as_number());
  return v;
}

inline Json access(const Json& r) {
  std::vector<std::vector<double>> lines;
  for (const auto& w : r["ways"].as_array()) {
    auto xy = coords(w);
    if (xy.size() >= 4) lines.push_back(std::move(xy));
  }
  std::vector<urban::transit::Place> places;
  for (const auto& p : r["places"].as_array()) places.push_back({p[0].as_number(), p[1].as_number(), p[2].as_number()});
  std::vector<uo::Space> spaces;
  for (const auto& s : r["spaces"].as_array()) {
    uo::Space sp{s["kind"].text(""), coords(s["ring"]), {}};
    if (s["holes"].is_array())
      for (const auto& h : s["holes"].as_array()) sp.holes.push_back(coords(h));
    if (sp.ring.size() >= 6) spaces.push_back(std::move(sp));
  }
  double area = num(r["area"], 0);
  uo::Box box;
  const bool boxed = r["box"].is_array();
  if (boxed) box = {r["box"][0].as_number(), r["box"][1].as_number(), r["box"][2].as_number(), r["box"][3].as_number()}, area = box.area();
  const auto walk = urban::transit::walking(lines, num(r["spacing"], 20));
  Json out = Json::array();
  std::vector<uo::Result> all;
  for (const auto& x : r["readings"].as_array()) {
    uo::Reading R;
    R.name = x["name"].text("reading");
    for (const auto& k : x["kinds"].as_array()) R.kinds.insert(k.text());
    R.least_area = num(x["least_area"], 0);
    R.metres = num(x["metres"], 400);
    R.along_network = !(x["along_network"].is_bool() && !x["along_network"].as_bool());
    R.source = x["source"].text("the request");
    all.push_back(uo::reach(walk, places, spaces, R, area, num(r["edge_spacing"], 25), num(r["cell"], 5), boxed ? &box : nullptr));
    const auto& a = all.back();
    out.push(Json::object({{"reading", a.reading}, {"share", a.share}, {"within", a.within}, {"people", a.people}, {"spaces", static_cast<double>(a.spaces)},
                           {"area", a.area}, {"area_share", a.area_share}, {"metres", R.metres}, {"least_area", R.least_area}, {"along_network", R.along_network},
                           {"source", R.source}}));
  }
  Json first = Json::array();
  if (!all.empty())
    for (std::size_t i = 0; i < places.size(); i++)
      first.push(Json::array({Json(places[i].x), Json(places[i].y), Json(places[i].people), finite(all[0].distance[i]), Json(static_cast<bool>(all[0].served[i]))}));
  return Json::object({{"readings", out}, {"places", first}, {"spaces", static_cast<double>(spaces.size())}});
}

inline bool handles(const std::string& op) { return op.rfind("openspace.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "openspace.access") return access(r);
  throw std::invalid_argument("openspace: no operation " + op);
}

}  // namespace science::openspace
