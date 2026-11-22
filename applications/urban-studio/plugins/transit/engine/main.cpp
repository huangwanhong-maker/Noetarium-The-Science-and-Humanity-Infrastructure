// The engine of the Urban Studio's plug-in "Public transport, open space and who can reach them" (ADR 0151, ADR 0154): every stop's
// service and SDG 11.2.1, and open space and SDG 11.7.1, under declared readings, on a place's walking ways, people, stops and open land. The composition is the API's (science/transit.hpp); the tools ask this
// program through science.transit.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   transit.access   {ways, places, stops, peak, readings, spacing}
//   openspace.access {ways, places, spaces, area, readings, spacing, cell}   (science/openspace.hpp, ADR 0154)
//   status           {}

#include <science/json.hpp>
#include <science/openspace.hpp>
#include <science/transit.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (science::openspace::handles(op)) return science::openspace::run(r);
  return science::transit::run(r);
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
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) ++i;  // a data root, which this engine does not read
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
