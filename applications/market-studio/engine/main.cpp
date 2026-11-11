// Market Studio's engine: the API's composition of the markets domain (api/cpp/science/markets.hpp) behind a program.
//
// It answers requests, one JSON object a line ({"op": …, …}), with one JSON line each ({"ok": true, "result": …} or
// {"ok": false, "error": …}): with --serve until its input ends; otherwise the one request given on its input, or as an
// argument. --data names the directory of the data library's products, the only files it reads (a city model's ways, for
// costs walked on streets).
//
// Nothing here names a place, a census or a column: the market is stated in the request by the tool that read the person's
// data (applications/market-studio/tools, or a script of their own through api/python/science/markets.py).

#include "science/markets.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>

namespace {

using science::Json;

std::string respond(const std::string& line, const std::string& data) {
  Json reply = Json::object();
  try {
    const Json r = Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", science::markets::answer(r, data));
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", false}, {"error", e.what()}});
  }
  return reply.dump();
}

}  // namespace

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  bool serve = false;
  std::string request, data = ".";
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--serve") == 0) serve = true;
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) data = argv[++i];
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) continue;
      std::cout << respond(line, data) << "\n" << std::flush;
    }
    return 0;
  }
  // One request: the whole of the input, which may be many megabytes (a market's pairs) on one line or several.
  if (request.empty()) request.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
  std::cout << respond(request, data) << "\n";
  return 0;
}
