// The engine of the plug-in "Architecture" (ADR 0089): what could be built on a block, anywhere the workbench holds streets and
// buildings. The composition is the API's (science/design.hpp), which a study may also run; this program answers the page with it.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   design  {at, streets, ways, buildings, rules, suns, night, generations?, seed?, least_floor?}   (science/design.hpp)
//   sunmap  {design}
//   status  {}

#include <science/design.hpp>
#include <science/json.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

science::design::Designer& designer() { static science::design::Designer d; return d; }

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "design") return designer().design(r);
  if (op == "sunmap") return designer().sunmap(r);
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
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) ++i;
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
