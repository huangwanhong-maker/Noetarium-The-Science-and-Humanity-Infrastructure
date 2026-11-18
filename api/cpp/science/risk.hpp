// The risks of a place, as one composition any work may call (ADR 0129): risk management, urban risk management and disaster risk
// put together on a place's roads, people, services and hazard maps. The Urban Science Workbench's plug-in "Lifelines and
// evacuation" answers its page with it, and a study may run it without the page. Nothing in it names a city.
//
//   criticality  {ways, frame, people, services, measure: {threshold, cap}, hazard, curve, areas, top}
//                each road link's criticality and risk over the hazard's scenarios (person-minutes added to one trip to the nearest service
//                by everyone, expected over the year's largest event),
//                each scenario's loss, and the link-disjoint paths from each area to the services with the links of its least cut
//   response     {ways, frame, people, stations, threshold, hazard, curve, candidates, k}
//                coverage by stations in each scenario, backup and sole coverage, and the k candidate sites that cover most
//   evacuation   {ways, frame, people, shelters, hazard, safe, walking, departure, warning, step, horizon, read, frames, candidates, k}
//                reach within the warning, the most housed, the bound at once and with departures, the crowd simulated, and shelters
//                chosen
//   financing    {losses, years, strategies, tower, return_periods, parametric}
//                strategies costed and compared, the cheapest tower, a parametric cover's basis risk (refused without an index)
//
// Conventions. Ways are [speed m/s, width m, x0, y0, x1, y1, …] in metres of a local frame (east, north) whose centre is
// `frame: {lat, lon}`; people [[x, y, count]]; services, stations and shelters [[x, y, capacity?, name?]]. A hazard is
// `{maps: [{return_period, path, box: [S, W, N, E], width, height, unit}]}`: a grid of bytes, rows from the south, the depth a byte
// times `unit` metres (the data library's flood and tsunami grids), read from the data root a caller gives, and `below: "linear"` to
// count the first map's state, falling linearly, over the years more frequent than it (nothing by default). The curve is "vehicles"
// (Pregnolato et al. 2017), "walking" (declared) or {depth: [...], share: [...]}.

#pragma once

#include <disaster_risk/disruption/disruption.hpp>
#include <disaster_risk/evacuation/evacuation.hpp>
#include <risk_management/criticality/criticality.hpp>
#include <risk_management/financing/financing.hpp>
#include <urban_risk_management/lifelines/lifelines.hpp>
#include <urban_risk_management/response/response.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <science/json.hpp>

namespace science::risk {

namespace ll = urban_risk_management::lifelines;
namespace ur = urban_risk_management::response;
namespace rc = risk_management::criticality;
namespace rf = risk_management::financing;
namespace dd = disaster_risk::disruption;
namespace de = disaster_risk::evacuation;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline double r1(double x) { return std::isfinite(x) ? std::round(x * 10) / 10 : x; }
inline double r3(double x) { return std::isfinite(x) ? std::round(x * 1000) / 1000 : x; }

inline std::vector<ll::Road> roads(const Json& ways) {
  std::vector<ll::Road> out;
  for (const auto& row : ways.as_array()) {
    const auto& r = row.as_array();
    if (r.size() < 6) continue;
    ll::Road road;
    road.speed = r[0].as_number();
    road.width = r[1].as_number();
    for (std::size_t i = 2; i + 1 < r.size(); i += 2) road.xy.push_back(r[i].as_number()), road.xy.push_back(r[i + 1].as_number());
    if (road.speed > 0 && road.width > 0) out.push_back(std::move(road));
  }
  if (out.empty()) throw std::invalid_argument("risk: no roads");
  return out;
}

inline std::vector<ll::Place> places(const Json& rows, double default_weight = 1) {
  std::vector<ll::Place> out;
  if (!rows.is_array()) return out;
  for (const auto& row : rows.as_array()) {
    const auto& r = row.as_array();
    if (r.size() < 2) continue;
    out.push_back({r[0].as_number(), r[1].as_number(), r.size() > 2 && r[2].is_number() ? r[2].as_number() : default_weight});
  }
  return out;
}

inline std::vector<std::string> names(const Json& rows) {
  std::vector<std::string> out;
  if (!rows.is_array()) return out;
  for (const auto& row : rows.as_array()) out.push_back(row.size() > 3 ? row[3].text() : std::string());
  return out;
}

inline dd::Frame frame_of(const Json& f) {
  if (!f.is_object()) throw std::invalid_argument("risk: a frame {lat, lon}");
  return {f["lat"].as_number(), f["lon"].as_number()};
}

inline dd::Curve curve_of(const Json& c, const std::string& fallback) {
  if (c.is_object()) {
    dd::Curve k{"declared", {}, {}, "the request"};
    for (const auto& x : c["depth"].as_array()) k.depth.push_back(x.as_number());
    for (const auto& x : c["share"].as_array()) k.share.push_back(x.as_number());
    k.check();
    return k;
  }
  const std::string name = c.text(fallback);
  if (name == "walking") return dd::walking_declared();
  if (name == "vehicles") return dd::pregnolato_2017();
  throw std::invalid_argument("risk: a curve is \"vehicles\", \"walking\" or {depth, share}");
}

struct Map {
  double return_period = 0;
  urban::exposure::Grid grid;
};

inline std::string joined(const std::string& root, const std::string& path) {
  if (path.empty()) throw std::invalid_argument("risk: a map names its file");
  if (path.find("..") != std::string::npos) throw std::invalid_argument("risk: a map's path stays within the data");
  if (path[0] == '/' || (path.size() > 1 && path[1] == ':')) return path;
  return root.empty() ? path : root + "/" + path;
}

inline std::vector<Map> maps_of(const Json& hazard, const std::string& root) {
  std::vector<Map> out;
  if (!hazard.is_object() || !hazard["maps"].is_array()) return out;
  for (const auto& m : hazard["maps"].as_array()) {
    Map map;
    map.return_period = num(m["return_period"], 0);
    const auto& box = m["box"];
    map.grid.south = box[0].as_number(), map.grid.west = box[1].as_number(), map.grid.north = box[2].as_number(), map.grid.east = box[3].as_number();
    map.grid.width = static_cast<std::size_t>(m["width"].as_number());
    map.grid.height = static_cast<std::size_t>(m["height"].as_number());
    map.grid.unit = num(m["unit"], 0.05);
    std::ifstream in(joined(root, m["path"].text()), std::ios::binary);
    if (!in) throw std::invalid_argument("risk: no map at " + m["path"].text());
    map.grid.values.resize(map.grid.width * map.grid.height);
    in.read(reinterpret_cast<char*>(map.grid.values.data()), static_cast<std::streamsize>(map.grid.values.size()));
    if (static_cast<std::size_t>(in.gcount()) != map.grid.values.size()) throw std::invalid_argument("risk: a map shorter than its width × height");
    map.grid.check();
    out.push_back(std::move(map));
  }
  std::sort(out.begin(), out.end(), [](const Map& a, const Map& b) { return a.return_period < b.return_period; });
  return out;
}

// The scenarios of a hazard on a graph: the undisturbed state and one per map, weighted over the curve (or, with one map and no
// return period, that map alone with weight 1).
struct Hazard {
  std::vector<rc::Scenario> scenarios;
  std::vector<dd::LinkDepth> depths;
  std::vector<double> return_periods;
};

inline Hazard hazard_on(const ll::Graph& g, const std::vector<Map>& maps, const dd::Frame& f, const dd::Curve& curve, bool linear_below = false) {
  Hazard h;
  if (maps.empty()) {
    h.scenarios.push_back({"undisturbed", 1, {}});
    return h;
  }
  std::vector<ll::State> states;
  for (const auto& m : maps) {
    h.depths.push_back(dd::depths(g, m.grid, f, 5));
    states.push_back(dd::state(h.depths.back(), curve));
    h.return_periods.push_back(m.return_period);
  }
  bool periods = true;
  for (double T : h.return_periods) periods = periods && T > 1;
  if (periods) h.scenarios = dd::scenarios(h.return_periods, states, linear_below);
  else
    for (std::size_t k = 0; k < states.size(); k++) h.scenarios.push_back({"map " + std::to_string(k + 1), 1.0 / states.size(), states[k]});
  return h;
}

inline Json polyline(const ll::Graph& g, const ll::Link& L) {
  Json xy = Json::array();
  for (std::size_t p : L.points) xy.push(r1(g.x[p])), xy.push(r1(g.y[p]));
  return xy;
}

inline Json links_json(const ll::Graph& g) {
  Json out = Json::array();
  for (const auto& L : g.links) out.push(polyline(g, L));
  return out;
}

// Junctions inside a polygon (local metres, even–odd rule).
inline std::vector<std::size_t> inside(const ll::Graph& g, const Json& polygon) {
  std::vector<double> xs, ys;
  const auto& a = polygon.as_array();
  for (std::size_t i = 0; i + 1 < a.size(); i += 2) xs.push_back(a[i].as_number()), ys.push_back(a[i + 1].as_number());
  std::vector<std::size_t> out;
  for (std::size_t j = 0; j < g.junctions(); j++) {
    const double x = g.x[g.point_of[j]], y = g.y[g.point_of[j]];
    bool in = false;
    for (std::size_t i = 0, k = xs.size() - 1; i < xs.size(); k = i++)
      if ((ys[i] > y) != (ys[k] > y) && x < (xs[k] - xs[i]) * (y - ys[i]) / (ys[k] - ys[i]) + xs[i]) in = !in;
    if (in) out.push_back(j);
  }
  return out;
}

// ---- criticality ------------------------------------------------------------------------------------------------------------------

inline Json criticality(const Json& r, const std::string& root) {
  const auto g = ll::graph_of(roads(r["ways"]));
  const auto f = frame_of(r["frame"]);
  ll::Measure m;
  m.threshold = num(r["measure"]["threshold"], 30 * 60);
  m.cap = num(r["measure"]["cap"], 120 * 60);
  const auto people = places(r["people"]), services = places(r["services"]);
  if (people.empty() || services.empty()) throw std::invalid_argument("risk: criticality needs people and services");
  const ll::Lifelines L(g, people, services, m);
  const auto H = hazard_on(g, maps_of(r["hazard"], root), f, curve_of(r["curve"], "vehicles"), r["hazard"]["below"].text() == "linear");
  const auto A = L.assess(H.scenarios);
  Json scen = Json::array();
  for (std::size_t k = 0; k < H.scenarios.size(); k++) {
    const ll::State phi = H.scenarios[k].state.empty() ? ll::State(g.links.size(), 1.0) : H.scenarios[k].state;
    const auto R = L.read(phi);
    std::size_t closed = 0, slowed = 0;
    for (double x : phi) closed += x == 0, slowed += x > 0 && x < 1;
    scen.push(Json::object({{"name", H.scenarios[k].name}, {"weight", H.scenarios[k].weight}, {"loss", r1(R.loss)}, {"beyond", r1(R.beyond)},
                            {"isolated", r1(R.isolated)}, {"mean_minutes", r3(R.mean_minutes)}, {"closed", closed}, {"slowed", slowed}}));
  }
  Json crit = Json::array(), risk = Json::array(), depth = Json::array();
  for (const auto& I : A.components) crit.push(r3(I.criticality)), risk.push(r3(I.risk));
  if (!H.depths.empty())
    for (double d : H.depths.back().deepest) depth.push(r3(d));
  const auto order = A.ranked(&rc::Importance::risk);
  const auto by_crit = A.ranked(&rc::Importance::criticality);
  const std::size_t top = static_cast<std::size_t>(num(r["top"], 20));
  const auto row = [&](std::size_t c) {
    const auto& I = A.components[c];
    const auto& Lk = g.links[c];
    return Json::object({{"link", c}, {"length", r1(Lk.length)}, {"criticality", r3(I.criticality)}, {"risk", r3(I.risk)}, {"birnbaum", r3(I.birnbaum)},
                         {"raw", r3(I.raw)}, {"fussell_vesely", r3(I.fussell_vesely)}});
  };
  Json top_risk = Json::array(), top_crit = Json::array();
  for (std::size_t i = 0; i < std::min(top, order.size()); i++) top_risk.push(row(order[i]));
  for (std::size_t i = 0; i < std::min(top, by_crit.size()); i++) top_crit.push(row(by_crit[i]));
  Json areas = Json::array();
  if (r["areas"].is_array()) {
    const ll::State intact(g.links.size(), 1.0);
    const ll::State worst = H.scenarios.back().state.empty() ? intact : H.scenarios.back().state;
    for (const auto& poly : r["areas"].as_array()) {
      const auto J = inside(g, poly["polygon"]);
      Json a = Json::object({{"name", poly["name"].text()}, {"junctions", J.size()}});
      if (!J.empty()) {
        const auto R0 = ll::redundancy(g, intact, J, L.services()), R1 = ll::redundancy(g, worst, J, L.services());
        Json cut = Json::array();
        for (std::size_t l : R0.cut) cut.push(l);
        a.set("paths", R0.holds_service ? Json("holds a service") : Json(R0.paths));
        a.set("paths_worst", R1.holds_service ? Json("holds a service") : Json(R1.paths));
        a.set("cut", cut);
      }
      areas.push(a);
    }
  }
  return Json::object({{"links", links_json(g)}, {"criticality", crit}, {"risk", risk}, {"depth", depth}, {"scenarios", scen},
                       {"expected", r3(A.expected)}, {"top_risk", top_risk}, {"top_criticality", top_crit}, {"areas", areas},
                       {"junctions", g.junctions()}, {"people", people.size()}, {"services", services.size()}, {"evaluations", A.evaluations},
                       {"measure", Json::object({{"threshold", m.threshold}, {"cap", m.cap}})}});
}

// ---- response ---------------------------------------------------------------------------------------------------------------------

inline Json response(const Json& r, const std::string& root) {
  const auto g = ll::graph_of(roads(r["ways"]));
  const auto f = frame_of(r["frame"]);
  const auto people = places(r["people"]), stations = places(r["stations"]);
  const double threshold = num(r["threshold"], 8 * 60);
  const ur::Response R(g, people, stations, threshold);
  const auto H = hazard_on(g, maps_of(r["hazard"], root), f, curve_of(r["curve"], "vehicles"), r["hazard"]["below"].text() == "linear");
  Json scen = Json::array();
  for (const auto& s : H.scenarios) {
    const ll::State phi = s.state.empty() ? ll::State(g.links.size(), 1.0) : s.state;
    const auto C = R.cover(phi);
    Json reach = Json::array(), only = Json::array();
    for (double x : C.reach) reach.push(r1(x));
    for (double x : C.only) only.push(r1(x));
    scen.push(Json::object({{"name", s.name}, {"weight", s.weight}, {"people", r1(C.people)}, {"covered", r1(C.covered)}, {"backup", r1(C.backup)},
                            {"share", r3(C.share)}, {"reach", reach}, {"only", only}}));
  }
  Json out = Json::object({{"scenarios", scen}, {"threshold", threshold}, {"stations", stations.size()}});
  const auto candidates = places(r["candidates"]);
  if (!candidates.empty()) {
    const auto S = R.site(candidates, static_cast<std::size_t>(num(r["k"], 3)), H.scenarios);
    Json chosen = Json::array(), value = Json::array();
    for (std::size_t c : S.chosen) chosen.push(c);
    for (double v : S.value) value.push(r1(v));
    out.set("siting", Json::object({{"chosen", chosen}, {"value", value}, {"existing", r1(S.existing)}, {"bound", r1(S.bound)}, {"guarantee", r3(S.guarantee)}}));
  }
  return out;
}

// ---- evacuation -------------------------------------------------------------------------------------------------------------------

inline de::Walking walking_of(const Json& w) {
  de::Walking out;
  out.speed = num(w["speed"], out.speed);
  out.gap_speed = num(w["gap_speed"], out.speed);
  out.capacity = num(w["capacity"], out.capacity);
  out.jam = num(w["jam"], out.jam);
  out.width_share = num(w["width_share"], out.width_share);
  out.least_width = num(w["least_width"], out.least_width);
  out.check();
  return out;
}

inline de::Departure departure_of(const Json& d) {
  if (!d.is_object()) return de::Departure::at_once();
  de::Departure out{{}, {}, d["source"].text("declared")};
  for (const auto& x : d["time"].as_array()) out.time.push_back(x.as_number());
  for (const auto& x : d["share"].as_array()) out.share.push_back(x.as_number());
  out.check();
  return out;
}

inline Json evacuation(const Json& r, const std::string& root) {
  const auto g = ll::graph_of(roads(r["ways"]));
  const auto f = frame_of(r["frame"]);
  const auto w = walking_of(r["walking"]);
  const auto dep = departure_of(r["departure"]);
  const double warning = num(r["warning"], 30 * 60);
  const auto maps = maps_of(r["hazard"], root);
  // The hazard's greatest map: who must leave (people where it is wet), which ground is safe (nodes where it is dry), and, if
  // asked, the links it closes on foot.
  const urban::exposure::Grid* zone = maps.empty() ? nullptr : &maps.back().grid;
  ll::State phi(g.links.size(), 1.0);
  if (zone && r["closes"].is_bool() && r["closes"].as_bool()) phi = dd::state(dd::depths(g, *zone, f, 5), dd::walking_declared());
  const auto net = de::network(g, phi, w, num(r["least"], 5));
  auto everyone = places(r["people"]);
  std::vector<ll::Place> people;
  double outside = 0;
  for (const auto& p : everyone) {
    const bool wet = !zone || zone->at(f.lat(p.y), f.lon(p.x)) > 0;
    if (wet || !(r["only_in_zone"].is_bool() && r["only_in_zone"].as_bool())) people.push_back(p);
    else outside += p.weight;
  }
  if (people.empty()) throw std::invalid_argument("risk: no one to evacuate");
  const auto P = de::people_on(g, net, people, w);
  std::vector<char> safe(net.x.size(), 0);
  std::size_t safe_nodes = 0;
  if (zone && r["safe"].text() == "outside")
    for (std::size_t v = 0; v < net.x.size(); v++) {
      const double d = zone->at(f.lat(net.y[v]), f.lon(net.x[v]));
      safe[v] = d == 0 && !net.incident[v].empty();
      safe_nodes += safe[v];
    }
  const auto shelters = places(r["shelters"], 0);
  const auto S = de::sinks_of(g, net, shelters, safe, w, names(r["shelters"]));
  if (S.node.empty()) throw std::invalid_argument("risk: no shelter and no safe ground");
  const auto R = de::reach(net, P, S, w, warning);
  const double horizon = num(r["horizon"], std::max(2 * warning, 3600.0));
  // A step that keeps the time-expanded network within reach of memory.
  const double step = std::max(num(r["step"], 15), std::ceil(horizon * static_cast<double>(net.x.size()) / 6e5));
  const auto B0 = de::bound(net, P, S, w, de::Departure::at_once(), step, horizon);
  const auto B = de::bound(net, P, S, w, dep, step, horizon);
  const auto Sim = de::simulate(net, P, S, w, dep, horizon, num(r["read"], 30), num(r["frames"], 60), num(r["dx"], 10), num(r["cleared"], 0.99));
  Json times = Json::array(), safe_sim = Json::array(), departed = Json::array(), bound0 = Json::array(), bound1 = Json::array();
  for (std::size_t k = 0; k < Sim.times.size(); k++) {
    times.push(r1(Sim.times[k]));
    safe_sim.push(r1(Sim.safe[k]));
    departed.push(r1(Sim.departed[k]));
    bound0.push(r1(std::min(B0.at(Sim.times[k]), B0.total)));
    bound1.push(r1(std::min(B.at(Sim.times[k]), B.total)));
  }
  Json shelters_out = Json::array();
  for (std::size_t k = 0; k < S.node.size(); k++) {
    if (S.name[k] == "safe ground") continue;
    shelters_out.push(Json::object({{"name", S.name[k]}, {"capacity", std::isfinite(S.capacity[k]) ? Json(S.capacity[k]) : Json()}, {"x", r1(net.x[S.node[k]])},
                                    {"y", r1(net.y[S.node[k]])}, {"housed", R.load.empty() || !std::isfinite(R.load[k]) ? Json() : Json(r1(R.load[k]))}}));
  }
  Json edges = Json::array(), peak = Json::array(), crowded = Json::array();
  for (std::size_t e = 0; e < net.edges.size(); e++) {
    edges.push(Json::object({{"link", net.edges[e].link}, {"width", r1(net.edges[e].width)}}));
    peak.push(r3(Sim.peak[e]));
    crowded.push(r1(Sim.crowded[e]));
  }
  Json frames = Json::array();
  for (std::size_t k = 0; k < Sim.frames.size(); k++) {
    Json fr = Json::array();
    for (float x : Sim.frames[k]) fr.push(std::round(static_cast<double>(x) * 100) / 100);
    frames.push(Json::object({{"t", r1(Sim.frame_times[k])}, {"density", fr}}));
  }
  Json late = Json::array();  // places that cannot walk to safety within the warning, with their people
  for (std::size_t i = 0; i < people.size(); i++)
    if (!(R.time[i] <= warning)) late.push(Json::array({r1(people[i].x), r1(people[i].y), r1(people[i].weight), std::isfinite(R.time[i]) ? Json(r1(R.time[i])) : Json()}));
  Json out = Json::object({{"links", links_json(g)}, {"edges", edges}, {"people", r1(R.people)}, {"outside", r1(outside)}, {"within", r1(R.within)},
                           {"housed", r1(R.housed)}, {"warning", warning}, {"safe_nodes", safe_nodes}, {"shelters", shelters_out}, {"late", late},
                           {"bound", Json::object({{"step", step}, {"clearance_at_once", B0.clearance}, {"clearance", B.clearance},
                                                   {"by_warning_at_once", r1(B0.at(warning))}, {"by_warning", r1(B.at(warning))}})},
                           {"simulation", Json::object({{"step", r3(Sim.step)}, {"total", r1(Sim.total)}, {"clearance", r1(Sim.clearance)}, {"cleared", Sim.cleared},
                                                        {"all_safe", r1(Sim.all_safe)}, {"reroutes", Sim.reroutes}, {"residual", Sim.residual},
                                                        {"by_warning", r1(Sim.safe.empty() ? 0.0 : [&] {
                                                           double v = 0;
                                                           for (std::size_t k = 0; k < Sim.times.size(); k++)
                                                             if (Sim.times[k] <= warning) v = Sim.safe[k];
                                                           return v;
                                                         }())}})},
                           {"curve", Json::object({{"t", times}, {"simulated", safe_sim}, {"departed", departed}, {"bound_at_once", bound0}, {"bound", bound1}})},
                           {"peak", peak}, {"crowded", crowded}, {"frames", frames},
                           {"walking", Json::object({{"speed", w.speed}, {"capacity", w.capacity}, {"jam", w.jam}, {"width_share", w.width_share}})},
                           {"departure", Json::object({{"source", dep.source}})}});
  const auto candidates = places(r["candidates"], 0);
  if (!candidates.empty()) {
    const de::Sinks none_more = S;
    const auto C = de::sinks_of(g, net, candidates, {}, w, names(r["candidates"]));
    const auto Pl = de::plan(net, P, S, C, w, warning, static_cast<std::size_t>(num(r["k"], 3)));
    Json chosen = Json::array(), value = Json::array();
    for (std::size_t c : Pl.chosen) chosen.push(c);
    for (double v : Pl.value) value.push(r1(v));
    out.set("plan", Json::object({{"chosen", chosen}, {"value", value}, {"existing", r1(Pl.existing)}, {"bound", r1(Pl.bound)}, {"guarantee", r3(Pl.guarantee)}}));
  }
  return out;
}

// ---- financing --------------------------------------------------------------------------------------------------------------------

inline rf::Kind kind_of(const std::string& s) {
  if (s == "budget") return rf::Kind::budget;
  if (s == "reserve") return rf::Kind::reserve;
  if (s == "contingent credit" || s == "credit") return rf::Kind::contingent_credit;
  if (s == "insurance") return rf::Kind::insurance;
  if (s == "catastrophe bond" || s == "bond") return rf::Kind::catastrophe_bond;
  return rf::Kind::other;
}

inline rf::Instrument instrument_of(const Json& j) {
  rf::Instrument i;
  i.name = j["name"].text("instrument");
  i.kind = kind_of(j["kind"].text());
  i.attachment = num(j["attachment"], 0);
  i.limit = num(j["limit"], rf::unbounded);
  i.fixed = num(j["fixed"], 0);
  i.rate = num(j["rate"], 0);
  i.multiple = num(j["multiple"], 0);
  i.unit = num(j["unit"], i.kind == rf::Kind::insurance || i.kind == rf::Kind::catastrophe_bond ? 0.0 : 1.0);
  return i;
}

inline std::vector<rf::Tier> tiers_of(const Json& j) {
  std::vector<rf::Tier> out;
  if (j.is_array())
    for (const auto& t : j.as_array()) out.push_back({num(t["up_to"], rf::unbounded), num(t["unit"], 1.5)});
  if (out.empty()) out.push_back({rf::unbounded, 1.5});
  out.back().up_to = rf::unbounded;
  return out;
}

inline Json cost_json(const rf::StrategyCost& c) {
  Json ins = Json::array();
  for (const auto& i : c.instruments)
    ins.push(Json::object({{"name", i.name}, {"kind", rf::name_of(i.kind)}, {"expected_paid", r1(i.expected_paid)}, {"attach_rate", r3(i.attach_rate * 1000) / 1000},
                           {"exhaust_rate", i.exhaust_rate}, {"annual", r1(i.annual)}, {"at_events", r1(i.at_events)}, {"expected", r1(i.expected)}}));
  Json at = Json::array();
  for (std::size_t k = 0; k < c.return_periods.size(); k++) {
    Json paid = Json::array();
    for (double p : c.paid_at[k]) paid.push(r1(p));
    at.push(Json::object({{"return_period", c.return_periods[k]}, {"loss", r1(c.loss_at[k])}, {"paid", paid}, {"unfunded", r1(c.unfunded_at[k])}, {"cost", r1(c.cost_at[k])}}));
  }
  return Json::object({{"name", c.name}, {"instruments", ins}, {"expected_loss", r1(c.expected_loss)}, {"expected_unfunded", r1(c.expected_unfunded)},
                       {"unfunded_cost", r1(c.unfunded_cost)}, {"expected", r1(c.expected)}, {"funded_share", r3(c.funded_share)}, {"at", at}});
}

inline Json financing(const Json& r) {
  std::vector<double> losses;
  for (const auto& x : r["losses"].as_array()) losses.push_back(x.as_number());
  const double years = num(r["years"], 0);
  if (losses.empty() || !(years > 0)) throw std::invalid_argument("risk: financing needs the events' losses and the years they span");
  std::vector<double> periods;
  if (r["return_periods"].is_array())
    for (const auto& x : r["return_periods"].as_array())
      if (x.as_number() <= years) periods.push_back(x.as_number());
  std::vector<rf::Strategy> strategies;
  if (r["strategies"].is_array())
    for (const auto& s : r["strategies"].as_array()) {
      rf::Strategy st{s["name"].text("strategy"), {}, tiers_of(s["unfunded"])};
      for (const auto& i : s["instruments"].as_array()) st.instruments.push_back(instrument_of(i));
      strategies.push_back(st);
    }
  Json out = Json::object({{"expected_loss", r1(tyche::descriptive::average_annual_loss(losses, years))}, {"events", losses.size()}, {"years", years}});
  if (!strategies.empty()) {
    const auto K = rf::compare(strategies, losses, years, periods);
    Json list = Json::array(), saving = Json::array();
    for (const auto& c : K.strategies) list.push(cost_json(c));
    for (double v : K.saving) saving.push(r1(v));
    out.set("baseline", cost_json(K.baseline));
    out.set("strategies", list);
    out.set("saving", saving);
    out.set("cheapest", K.cheapest);
  }
  if (r["tower"].is_object()) {
    const auto& t = r["tower"];
    std::vector<rf::Instrument> order;
    for (const auto& i : t["order"].as_array()) order.push_back(instrument_of(i));
    std::vector<double> cand;
    std::vector<double> rp;
    for (const auto& x : t["candidates"].as_array()) rp.push_back(x.as_number());
    std::vector<double> sorted(losses);
    std::vector<double> usable;
    for (double T : rp)
      if (T >= 1 && T <= years) usable.push_back(T);
    for (double v : tyche::descriptive::occurrence_curve(sorted, years, usable))
      if (v > 0) cand.push_back(v);
    rf::TowerOptions o;
    o.cap_period = num(t["cap_period"], 0);
    o.cap = num(t["cap"], rf::unbounded);
    o.top_unbounded = t["top_unbounded"].is_bool() && t["top_unbounded"].as_bool();
    const auto T = rf::best_tower(order, cand, tiers_of(t["unfunded"]), losses, years, o);
    Json layers = Json::array();
    for (const auto& i : T.strategy.instruments)
      layers.push(Json::object({{"name", i.name}, {"kind", rf::name_of(i.kind)}, {"attachment", r1(i.attachment)}, {"limit", std::isfinite(i.limit) ? Json(r1(i.limit)) : Json()}}));
    Json cj = Json::array();
    for (double c : cand) cj.push(r1(c));
    out.set("tower", Json::object({{"layers", layers}, {"cost", cost_json(rf::cost(T.strategy, losses, years, periods))}, {"tried", T.tried}, {"feasible", T.feasible}, {"candidates", cj}}));
  }
  if (r["parametric"].is_object()) {
    const auto& p = r["parametric"];
    if (!p["index"].is_array() || p["index"].size() != losses.size())
      out.set("parametric", Json::object({{"refused", "a parametric cover needs an index for every event; this catalogue gives its losses alone, and none is invented"}}));
    else {
      std::vector<double> idx;
      for (const auto& x : p["index"].as_array()) idx.push_back(x.as_number());
      const auto Q = rf::parametric(idx, losses, years, num(p["attachment"], 0), num(p["limit"], 1), num(p["multiple"], 1.5));
      out.set("parametric", Json::object({{"trigger", Q.payout.trigger}, {"exhaustion", Q.payout.exhaustion}, {"limit", Q.payout.limit}, {"expected_paid", r1(Q.expected_paid)},
                                          {"premium", r1(Q.premium)}, {"covered", r1(Q.covered)}, {"rmse", r1(Q.risk.rmse)}, {"shortfall", r1(Q.risk.shortfall)},
                                          {"excess", r1(Q.risk.excess)}, {"missed", Q.risk.missed}, {"unearned", Q.risk.unearned}, {"correlation", r3(Q.risk.correlation)}}));
    }
  }
  return out;
}

inline Json run(const Json& r, const std::string& data_root = {}) {
  const std::string op = r["op"].text();
  if (op == "criticality") return criticality(r, data_root);
  if (op == "response") return response(r, data_root);
  if (op == "evacuation") return evacuation(r, data_root);
  if (op == "financing") return financing(r);
  throw std::invalid_argument("risk: no operation \"" + op + "\"");
}

}  // namespace science::risk
