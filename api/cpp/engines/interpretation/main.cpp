// The engine of derivability under interpretation: the API's composition of apodeixis's `interpretation` and tekmerion's
// interpreters (api/cpp/science/interpretation.hpp), behind a program, for scripts (api/python/science/interpret.py) and for any
// work that has no engine of its own for it.
//
// It answers one JSON request on its input, or as an argument, with one JSON line ({"ok": true, "result": …} or {"ok": false,
// "error": …}); with --serve, a request a line until its input ends. It reads no file: a request holds the whole case and every
// interpreter's statement, a model's proposal among them as data.

#include "science/interpretation.hpp"

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
    reply.set("result", science::interpretation::answer(r));
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
