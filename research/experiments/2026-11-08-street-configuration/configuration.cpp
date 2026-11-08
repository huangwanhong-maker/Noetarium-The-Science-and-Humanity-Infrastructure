// The configuration of Melbourne's streets, read at its pedestrian counters (ADR 0084).
//
// For each network of inputs.json (prep.py), the street graph (urban-science's street_graph: points within 1 m one, bends over 20°
// kept), its segment map, and each configuration of the plan: the streets at every cost (angular, metric, topological) and radius
// (400, 800, 1200, 2000 m and n); the walking network, angular at 800 m and n; the roads without lanes, angular at 400, 800, 1200 m
// and n. For each counter, the nearest segment and its measures.
// Writes measures.json (per counter, committed) and maps.json (the segments and two of their configurations, for the figures; not
// committed, as it carries the ways, ODbL).
//
//   configuration <inputs.json> <results directory> [--threads N]

#include <urban/syntax/configuration.hpp>
#include <urban/syntax/syntax.hpp>

#include <science/json.hpp>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using science::Json;
using urban::syntax::Cost;

namespace {

std::string read_file(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("cannot read " + path);
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

const char* name(Cost c) { return c == Cost::metric ? "metric" : c == Cost::angular ? "angular" : "topological"; }

Json numbers(const std::vector<double>& v, double scale) {
  Json a = Json::array();
  for (double x : v) a.push(Json(std::round(x * scale) / scale));
  return a;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "configuration <inputs.json> <results directory> [--threads N]\n";
    return 2;
  }
  std::size_t threads = std::max(1u, std::thread::hardware_concurrency());
  for (int i = 3; i + 1 < argc; i++)
    if (std::string(argv[i]) == "--threads") threads = static_cast<std::size_t>(std::stoul(argv[i + 1]));
  const Json in = Json::parse(read_file(argv[1]));
  const std::string out = argv[2];
  const double n = std::numeric_limits<double>::infinity();
  struct Plan {
    std::string network;
    std::vector<Cost> costs;
    std::vector<double> radii;
  };
  const std::vector<Plan> plans{{"streets", {Cost::angular, Cost::metric, Cost::topological}, {400, 800, 1200, 2000, n}},
                                {"walking", {Cost::angular}, {800, n}},
                                {"roads", {Cost::angular}, {400, 800, 1200, n}}};
  Json networks = Json::object(), maps = Json::object();
  for (const auto& plan : plans) {
    std::vector<std::vector<double>> ways;
    const Json& w = in["networks"][plan.network];
    for (std::size_t k = 0; k < w.size(); k++) {
      std::vector<double> xy;
      for (std::size_t i = 0; i < w[k].size(); i++) xy.push_back(w[k][i].as_number());
      ways.push_back(std::move(xy));
    }
    const auto t0 = std::chrono::steady_clock::now();
    const auto g = urban::syntax::street_graph(ways, 1.0, 20);
    const auto m = urban::syntax::segment_map(g);
    std::cerr << plan.network << ": " << g.size() << " junctions, " << m.size() << " segments\n";
    // The counters' segments, once.
    std::vector<urban::syntax::Nearest> near;
    const Json& pts = in["points"];
    for (std::size_t k = 0; k < pts.size(); k++) near.push_back(urban::syntax::nearest_segment(m, {pts[k]["x"].as_number(), pts[k]["y"].as_number()}));
    Json runs = Json::array();
    for (const Cost cost : plan.costs)
      for (const double radius : plan.radii) {
        const auto t1 = std::chrono::steady_clock::now();
        const auto c = urban::syntax::configuration(m, cost, radius, threads);
        const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t1).count();
        Json points = Json::array();
        for (std::size_t k = 0; k < near.size(); k++) {
          const std::size_t s = near[k].segment;
          points.push(Json::object({{"id", pts[k]["id"]}, {"segment", Json(static_cast<double>(s))}, {"distance", Json(std::round(near[k].distance * 100) / 100)},
                                    {"nach", Json(c.nach[s])}, {"integration", Json(c.integration[s])}, {"choice", Json(c.choice[s])},
                                    {"meanDepth", Json(c.mean_depth[s])}, {"nodeCount", Json(c.node_count[s])}}));
        }
        runs.push(Json::object({{"cost", Json(name(cost))}, {"radius", radius == n ? Json("n") : Json(radius)}, {"seconds", Json(seconds)}, {"points", points}}));
        std::cerr << "  " << name(cost) << " at " << (radius == n ? std::string("n") : std::to_string(static_cast<int>(radius))) << ": " << seconds << " s\n";
        if (plan.network == "streets" && cost == Cost::angular && (radius == n || radius == 800)) {
          Json seg = Json::array();
          if (!maps.has("segments")) {
            for (std::size_t i = 0; i < m.size(); i++)
              seg.push(Json::array({Json(std::round(m.a[i].real() * 10) / 10), Json(std::round(m.a[i].imag() * 10) / 10), Json(std::round(m.b[i].real() * 10) / 10),
                                    Json(std::round(m.b[i].imag() * 10) / 10)}));
            maps.set("segments", seg);
          }
          const std::string key = radius == n ? "angular_n" : "angular_800";
          maps.set(key, Json::object({{"nach", numbers(c.nach, 1000)}, {"integration", numbers(c.integration, 1000)}}));
        }
      }
    const double total = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    networks.set(plan.network, Json::object({{"junctions", Json(static_cast<double>(g.size()))}, {"segments", Json(static_cast<double>(m.size()))},
                                             {"seconds", Json(total)}, {"threads", Json(static_cast<double>(threads))}, {"runs", runs}}));
  }
  std::ofstream(out + "/measures.json") << Json::object({{"networks", networks}}).dump() << "\n";
  std::ofstream(out + "/maps.json") << maps.dump() << "\n";
  std::cerr << "written " << out << "/measures.json and maps.json\n";
  return 0;
}
