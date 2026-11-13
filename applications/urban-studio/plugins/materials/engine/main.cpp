// The plug-in of materials and flows: the API's composition of the industrial-ecology domain (api/cpp/science/materials.hpp), behind a
// program.
//
// It answers one JSON request on its input, or as an argument, with one JSON line ({"ok": true, "result": …} or {"ok": false,
// "error": …}); with --serve, a request a line until its input ends. Nothing here names a city or a database: buildings, a stock, a
// system of flows, a product system or an economy is stated in the request by the tool that read the person's data (the plug-in's
// tools/, or a script of one's own through api/python/science/materials.py).

#include "science/materials.hpp"
#include "science/water.hpp"

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
    reply.set("ok", Json(true));
    const std::string op = r["op"].text();
    reply.set("result", science::water::handles(op) ? science::water::answer(r) : science::materials::answer(r));
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", Json(false)}, {"error", Json(std::string(e.what()))}});
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
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) ++i;  // the data library's products: this engine reads none
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line))
      if (!line.empty()) std::cout << respond(line) << "\n" << std::flush;
    return 0;
  }
  if (request.empty()) request.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
  std::cout << respond(request) << "\n";
  return 0;
}
