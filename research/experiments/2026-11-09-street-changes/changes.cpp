// The street-changes study's computation (ADR 0087): any city's inputs (ways, sensors, footprints of changes, observations by
// period) through urban science's interventions and peira's interference. Nothing in it names a city.
//
//   changes <inputs.json> <results.json> [--bounds 100,400] [--replications 499] [--window 2]
//
// Inputs: {"ways": [[x0, y0, x1, y1, ...], ...], "sensors": [{"x", "y", ...}], "footprints": [{"xy": [...], "begin"}],
// "observations": [[sensor, period, value, ...]], "periods": [...]}; metres in a local frame. The ways become a network whose times
// are metres, its loose ends within 8 m stitched to points and to ways (tiles cut ways at their edges and simplify their lines).

#include <urban/access/access.hpp>
#include <urban/interventions/interventions.hpp>

#include <science/json.hpp>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using science::Json;
namespace ui = urban::interventions;

namespace {

std::string read_file(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("cannot read " + path);
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

Json number_or_null(double v) { return std::isfinite(v) ? Json(v) : Json(); }

Json staggered_json(const peira::quasi::StaggeredFit& f) {
  Json gt = Json::array(), ex = Json::array();
  for (const auto& g : f.effects)
    gt.push(Json::object({{"group", Json(static_cast<double>(g.group))}, {"period", Json(static_cast<double>(g.period))}, {"att", number_or_null(g.att)},
                          {"se", number_or_null(g.se)}, {"treated", Json(static_cast<double>(g.treated))}, {"comparison", Json(static_cast<double>(g.comparison))}}));
  for (const auto& [e, v] : f.by_exposure) {
    const auto it = f.by_exposure_se.find(e);
    ex.push(Json::array({Json(static_cast<double>(e)), number_or_null(v), number_or_null(it != f.by_exposure_se.end() ? it->second : NAN)}));
  }
  Json groups = Json::array();
  for (const auto& [g, n] : f.group_size) groups.push(Json::array({Json(static_cast<double>(g)), Json(static_cast<double>(n))}));
  return Json::object({{"simple", number_or_null(f.simple)}, {"simple_se", number_or_null(f.simple_se)}, {"by_exposure", ex}, {"groups", groups}, {"group_time", gt}});
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "changes <inputs.json> <results.json> [--bounds 100,400] [--replications 499] [--window 2]\n";
    return 2;
  }
  ui::Options o;
  o.replications = 499;
  for (int i = 3; i + 1 < argc; i += 2) {
    const std::string k = argv[i], v = argv[i + 1];
    if (k == "--bounds") {
      o.bounds.clear();
      std::stringstream ss(v);
      std::string part;
      while (std::getline(ss, part, ',')) o.bounds.push_back(std::stod(part));
    } else if (k == "--replications") o.replications = std::stoul(v);
    else if (k == "--window") o.window = std::stoul(v);
  }
  const auto t0 = std::chrono::steady_clock::now();
  const Json in = Json::parse(read_file(argv[1]));
  std::vector<urban::access::Way> ways;
  for (std::size_t k = 0; k < in["ways"].size(); k++) {
    urban::access::Way w;
    w.speed = 1;
    for (std::size_t i = 0; i < in["ways"][k].size(); i++) w.xy.push_back(in["ways"][k][i].as_number());
    if (w.xy.size() >= 4) ways.push_back(std::move(w));
  }
  auto n = urban::access::network_of(ways, 1.0);
  const std::size_t stitched = urban::access::stitch_ends(n, 8, 1) + urban::access::stitch_to_ways(n, 8, 1);
  std::vector<ui::Place> sensors;
  for (std::size_t k = 0; k < in["sensors"].size(); k++) sensors.push_back({in["sensors"][k]["x"].as_number(), in["sensors"][k]["y"].as_number()});
  std::vector<ui::Footprint> footprints;
  for (std::size_t k = 0; k < in["footprints"].size(); k++) {
    ui::Footprint f;
    const Json& xy = in["footprints"][k]["xy"];
    for (std::size_t i = 0; i < xy.size(); i++) f.xy.push_back(xy[i].as_number());
    f.begin = static_cast<std::int64_t>(in["footprints"][k]["begin"].as_number());
    footprints.push_back(std::move(f));
  }
  std::vector<ui::Observation> obs;
  for (std::size_t k = 0; k < in["observations"].size(); k++) {
    const Json& r = in["observations"][k];
    obs.push_back({static_cast<std::size_t>(r[0].as_number()), static_cast<std::int64_t>(r[1].as_number()), r[2].as_number()});
  }
  std::vector<std::int64_t> periods;
  for (std::size_t k = 0; k < in["periods"].size(); k++) periods.push_back(static_cast<std::int64_t>(in["periods"][k].as_number()));
  const auto ev = ui::evaluate(n, sensors, footprints, obs, periods, o);

  Json bounds = Json::array(), entering = Json::array(), two = Json::array(), rings = Json::array(), units = Json::array(), exposure = Json::array();
  for (double b : o.bounds) bounds.push(Json(b));
  for (std::size_t c : ev.entering) entering.push(Json(static_cast<double>(c)));
  for (std::size_t r = 0; r < o.bounds.size(); r++) {
    two.push(Json::object({{"effect", number_or_null(ev.two_way.effect[r])}, {"se", number_or_null(ev.two_way.se[r])}, {"p", number_or_null(ev.two_way.p[r])},
                           {"lower", number_or_null(ev.two_way.lower[r])}, {"upper", number_or_null(ev.two_way.upper[r])}}));
    rings.push(staggered_json(ev.by_ring[r]));
  }
  for (const auto& u : ev.units)
    units.push(Json::object({{"sensor", Json(static_cast<double>(u.unit))}, {"onset", Json(static_cast<double>(u.onset))}, {"ring", Json(static_cast<double>(u.ring))},
                             {"effect", number_or_null(u.effect)}, {"before", Json(static_cast<double>(u.before))}, {"after", Json(static_cast<double>(u.after))},
                             {"comparisons", Json(static_cast<double>(u.comparisons))}}));
  for (std::size_t i = 0; i < sensors.size(); i++) {
    Json path = Json::array();
    for (std::size_t t = 0; t < periods.size(); t++) path.push(Json(static_cast<double>(ev.exposure.at(i, t))));
    exposure.push(path);
  }
  Json contacts = Json::array();
  for (const auto& c : ev.contacts) contacts.push(Json::array({Json(static_cast<double>(c.unit)), Json(static_cast<double>(c.treatment)), Json(std::round(c.distance * 10) / 10)}));
  const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  const Json out = Json::object({{"bounds", bounds}, {"periods", in["periods"]}, {"entering", entering}, {"two_way", two}, {"staggered", rings}, {"units", units},
                                 {"rings", exposure}, {"contacts", contacts}, {"network", Json::object({{"points", Json(static_cast<double>(n.x.size()))}, {"stitched", Json(static_cast<double>(stitched))}})},
                                 {"replications", Json(static_cast<double>(o.replications))}, {"seconds", Json(seconds)}});
  std::ofstream(argv[2]) << out.dump();
  std::cout << "sensors " << sensors.size() << ", footprints " << footprints.size() << ", contacts " << ev.contacts.size() << "; entering ring 0.." << o.bounds.size()
            << ":";
  for (std::size_t c : ev.entering) std::cout << ' ' << c;
  std::cout << "; two-way:";
  for (std::size_t r = 0; r < o.bounds.size(); r++) std::cout << ' ' << ev.two_way.effect[r] << " (" << ev.two_way.se[r] << ")";
  std::cout << "; staggered:";
  for (std::size_t r = 0; r < o.bounds.size(); r++) std::cout << ' ' << ev.by_ring[r].simple << " (" << ev.by_ring[r].simple_se << ")";
  std::cout << "; " << seconds << " s\n";
  return 0;
}
