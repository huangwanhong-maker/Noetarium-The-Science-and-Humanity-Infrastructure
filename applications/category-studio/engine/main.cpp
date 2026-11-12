// The Category Studio's engine: the API's composition of kategoria's script language (api/cpp/science/categories.hpp)
// behind a program.
//
// It answers requests, one JSON object a line ({"op": "run", "script": …}), with one JSON line each ({"ok": true,
// "result": …} or {"ok": false, "error": …}): with --serve until its input ends; otherwise the one request given on its
// input, or as an argument. It reads no file and keeps nothing between requests: a script is the whole of what it is told.

#include "science/categories.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>

namespace {

using science::Json;

std::string respond(const std::string& line) {
  Json reply = Json::object();
  try {
    const Json r = Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", science::categories::answer(r));
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
  if (request.empty()) request.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
  std::cout << respond(request) << "\n";
  return 0;
}
