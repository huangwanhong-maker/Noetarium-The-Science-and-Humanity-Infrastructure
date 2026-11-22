// The engine of the Urban Studio's plug-in "Visitors, how many and when" (ADR 0156, ADR 0157): the visitor stock month by month and by
// origin under declared readings of "visitor"; what visitors' spending becomes in an economy, and what a levy raises and who bears it. The composition is the API's (science/visitors.hpp); the tools ask this program through
// science.visitors.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   visitors.stock   {places, readings, bands}
//   tourism.accounts {table, purchases, nights, units}   (science/tourism.hpp, ADR 0157)
//   tourism.levy     {market, levies, rates, cap, value_added_per_night}
//   status         {}

#include <science/json.hpp>
#include <science/tourism.hpp>
#include <science/visitors.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (science::tourism::handles(op)) return science::tourism::run(r);
  return science::visitors::run(r);
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
