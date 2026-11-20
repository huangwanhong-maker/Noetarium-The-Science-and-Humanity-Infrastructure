// The engine of the Urban Studio's plug-in "Allocation and justice" (ADR 0136): an allocation round run stage by stage with its
// reasons, judged against its alternatives under declared principles, its procedure kept in a sealed journal and checked, a budget
// decided by votes under several rules, a reserve system under every precedence, and who finally gains. The composition is the API's
// (science/allocation.hpp); the tools ask this program through science.allocation.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   run · judge · procedure · claims · budget · reserves · incidence · status

#include <science/allocation.hpp>
#include <science/json.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

Json answer(const Json& r) {
  if (r["op"].text() == "status") return Json::object({{"ok", true}});
  return science::allocation::run(r);
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
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) ++i;  // the data root every engine is given; this one reads no file
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
