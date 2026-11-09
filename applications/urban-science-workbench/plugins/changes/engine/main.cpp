// The engine of the plug-in "What a change did": what a change to a city's streets did to what is counted there, and near it (ADR 0087),
// on urban science's interventions and peira's interference, for any case the data library holds (data/products/street_changes.py).
// Nothing in it names a city: a case is a file of sensors, observations, footprints and ways.
//
// It answers requests, one JSON object a line ({"op": …, …}), with one JSON line each ({"ok": true, "result": …} or
// {"ok": false, "error": …}): with --serve until its input ends, as the workbench's server keeps it; otherwise the one request given as an
// argument. --data names the directory of the data library's products, of which it reads street-changes/.
//
//   cases    {}                         the cases built (street-changes/index.json)
//   evaluate {case, bounds?, replications?, window?}
//                         the contacts along the ways, each sensor's ring through the periods and its own change, the effects by ring in
//                         the two-way regression and by staggered adoption with their event studies, the footprints and the sensors'
//                         series, for drawing
//   status   {}                         the engine answers

#include <urban/access/access.hpp>
#include <urban/interventions/interventions.hpp>

#include <science/json.hpp>

#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

using science::Json;
namespace ui = urban::interventions;

std::string& data_dir() { static std::string d = "."; return d; }
double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }

std::string slurp(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("the data library has no " + path + ": python data/products/street_changes.py <case>");
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

Json finite(double v) { return std::isfinite(v) ? Json(v) : Json(); }

// A case read once and kept: its record and its network (metres), stitched.
struct Case {
  Json record;
  urban::access::Network metres;
  std::vector<ui::Place> sensors;
  std::vector<ui::Footprint> footprints;
  std::vector<ui::Observation> observations;
  std::vector<std::int64_t> periods;
};

const Case& load(const std::string& name) {
  static std::map<std::string, std::unique_ptr<Case>> kept;
  for (const char c : name)
    if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-')) throw std::invalid_argument("a case's name is letters, digits and hyphens");
  auto& slot = kept[name];
  if (slot) return *slot;
  auto c = std::make_unique<Case>();
  c->record = Json::parse(slurp(data_dir() + "/street-changes/" + name + ".json"));
  std::vector<urban::access::Way> ways;
  const Json& W = c->record["ways"];
  for (std::size_t k = 0; k < W.size(); k++) {
    urban::access::Way w;
    w.speed = 1;
    for (std::size_t i = 0; i < W[k].size(); i++) w.xy.push_back(W[k][i].as_number());
    if (w.xy.size() >= 4) ways.push_back(std::move(w));
  }
  c->metres = urban::access::network_of(ways, 1.0);
  urban::access::stitch_ends(c->metres, 8, 1);
  urban::access::stitch_to_ways(c->metres, 8, 1);
  const Json& S = c->record["sensors"];
  for (std::size_t k = 0; k < S.size(); k++) c->sensors.push_back({S[k]["x"].as_number(), S[k]["y"].as_number()});
  const Json& F = c->record["footprints"];
  for (std::size_t k = 0; k < F.size(); k++) {
    ui::Footprint f;
    for (std::size_t i = 0; i < F[k]["xy"].size(); i++) f.xy.push_back(F[k]["xy"][i].as_number());
    f.begin = static_cast<std::int64_t>(F[k]["begin"].as_number());
    c->footprints.push_back(std::move(f));
  }
  const Json& O = c->record["observations"];
  for (std::size_t k = 0; k < O.size(); k++)
    c->observations.push_back({static_cast<std::size_t>(O[k][0].as_number()), static_cast<std::int64_t>(O[k][1].as_number()), O[k][2].as_number()});
  for (std::size_t k = 0; k < c->record["periods"].size(); k++) c->periods.push_back(static_cast<std::int64_t>(c->record["periods"][k].as_number()));
  slot = std::move(c);
  return *slot;
}

Json staggered_json(const peira::quasi::StaggeredFit& f) {
  Json ex = Json::array();
  for (const auto& [e, v] : f.by_exposure) {
    const auto it = f.by_exposure_se.find(e);
    ex.push(Json::array({Json(static_cast<double>(e)), finite(v), finite(it != f.by_exposure_se.end() ? it->second : NAN)}));
  }
  std::size_t units = 0;
  for (const auto& [g, n] : f.group_size) units += n;
  return Json::object({{"simple", finite(f.simple)}, {"se", finite(f.simple_se)}, {"units", Json(static_cast<double>(units))}, {"by_exposure", ex}});
}

Json op_evaluate(const Json& r) {
  const std::string name = r["case"].text();
  const Case& c = load(name);
  ui::Options o;
  o.bounds.clear();
  const Json& b = r.has("bounds") ? r["bounds"] : c.record["bounds"];
  for (std::size_t k = 0; k < b.size(); k++) o.bounds.push_back(b[k].as_number());
  o.replications = static_cast<std::size_t>(number_or(r, "replications", 199));
  o.window = static_cast<std::size_t>(number_or(r, "window", c.record["period"].text() == "year" ? 2 : 12));
  const auto ev = ui::evaluate(c.metres, c.sensors, c.footprints, c.observations, c.periods, o);
  const std::size_t R = o.bounds.size();
  Json rings = Json::array();
  for (std::size_t k = 0; k < R; k++)
    rings.push(Json::object({{"bound", Json(o.bounds[k])}, {"entering", Json(static_cast<double>(ev.entering[k]))},
                             {"two_way", Json::object({{"effect", finite(ev.two_way.effect[k])}, {"se", finite(ev.two_way.se[k])}, {"lower", finite(ev.two_way.lower[k])},
                                                       {"upper", finite(ev.two_way.upper[k])}, {"p", finite(ev.two_way.p[k])}})},
                             {"staggered", staggered_json(ev.by_ring[k])}}));
  // The sensors: where they are, their ring through the periods, their own change, their series.
  std::vector<Json> series(c.sensors.size(), Json::array());
  for (const auto& ob : c.observations) series[ob.unit].push(Json::array({Json(static_cast<double>(ob.period)), Json(std::round(ob.value * 1e4) / 1e4)}));
  std::vector<const ui::UnitEffect*> own(c.sensors.size(), nullptr);
  for (const auto& u : ev.units) own[u.unit] = &u;
  Json sensors = Json::array();
  const Json& S = c.record["sensors"];
  for (std::size_t i = 0; i < c.sensors.size(); i++) {
    Json path = Json::array();
    for (std::size_t t = 0; t < c.periods.size(); t++) path.push(Json(static_cast<double>(ev.exposure.at(i, t))));
    sensors.push(Json::object({{"name", S[i]["name"]}, {"lat", S[i]["lat"]}, {"lon", S[i]["lon"]}, {"entry", Json(static_cast<double>(ev.exposure.entry(i)))},
                               {"onset", ev.exposure.reached(i) == ui::pi::never ? Json() : Json(static_cast<double>(ev.exposure.reached(i)))},
                               {"own", own[i] ? finite(own[i]->effect) : Json()}, {"rings", path}, {"series", series[i]}}));
  }
  // The footprints that reach a sensor, and the others, for drawing.
  std::vector<char> reaches(c.footprints.size(), 0);
  for (const auto& ct : ev.contacts) reaches[ct.treatment] = 1;
  Json footprints = Json::array();
  const Json& F = c.record["footprints"];
  for (std::size_t k = 0; k < c.footprints.size(); k++)
    footprints.push(Json::object({{"ll", F[k]["ll"]}, {"begin", F[k]["begin"]}, {"label", F[k]["label"]}, {"kind", F[k]["kind"]}, {"reaches", Json(static_cast<bool>(reaches[k]))}}));
  Json bounds = Json::array();
  for (double v : o.bounds) bounds.push(Json(v));
  return Json::object({{"case", Json(name)}, {"city", c.record["city"]}, {"question", c.record["question"]}, {"outcome", c.record["outcome"]}, {"period", c.record["period"]},
                       {"periods", c.record["periods"]}, {"bounds", bounds}, {"never", Json(static_cast<double>(ev.entering[R]))}, {"rings", rings}, {"sensors", sensors},
                       {"footprints", footprints}, {"contacts", Json(static_cast<double>(ev.contacts.size()))}, {"replications", Json(static_cast<double>(o.replications))},
                       {"source", c.record["source"]}});
}

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "cases") return Json::parse(slurp(data_dir() + "/street-changes/index.json"));
  if (op == "evaluate") return op_evaluate(r);
  if (op == "status") return Json::object({{"ok", true}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

std::string respond(const std::string& line) {
  Json reply = Json::object();
  try {
    const Json r = Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", answer(r));
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", false}, {"error", e.what()}});
  }
  return reply.dump();
}

}  // namespace

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  bool serve = false;
  std::string request;
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--serve") == 0) serve = true;
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) data_dir() = argv[++i];
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) continue;
      std::cout << respond(line) << "\n" << std::flush;
    }
    return 0;
  }
  if (request.empty()) std::getline(std::cin, request);
  std::cout << respond(request) << "\n";
  return 0;
}
