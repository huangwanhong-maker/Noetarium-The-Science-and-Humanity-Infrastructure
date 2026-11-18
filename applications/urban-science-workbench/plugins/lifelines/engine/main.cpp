// The engine of the plug-in "Lifelines and evacuation" (ADR 0129): which roads matter most to reaching a hospital under a hazard,
// who stays within reach of a station, how a place's people walk out of a tsunami's or a flood's reach, and how its losses are
// financed. The composition is the API's (science/risk.hpp), which a study may also run; this program answers the page with it.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends. Hazard maps are read from
// the data library's products, under --data.
//
//   criticality  {ways, frame, people, services, measure, hazard, curve, areas, top}
//   response     {ways, frame, people, stations, threshold, hazard, curve, candidates, k}
//   evacuation   {ways, frame, people, shelters, hazard, safe, walking, departure, warning, step, horizon, candidates, k}
//   financing    {losses, years, strategies, tower, return_periods, parametric}
//   status       {}

#include <science/json.hpp>
#include <science/risk.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;
std::string data_root;

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  return science::risk::run(r, data_root);
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
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) data_root = argv[++i];
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
