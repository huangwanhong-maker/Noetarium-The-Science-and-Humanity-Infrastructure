// The engine of the Urban Studio's plug-in "A region's economy" (ADR 0166): the structure of regional economies and how it changed. The
// composition is the API's (science/diversification.hpp); the tools ask this program through science.diversification.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   diversification.assess {table, threshold, base, opportunities}
//   status                 {}

#include <science/diversification.hpp>
#include <science/json.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  return science::diversification::run(r);
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
