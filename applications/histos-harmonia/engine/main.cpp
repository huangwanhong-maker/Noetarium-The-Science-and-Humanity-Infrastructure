// Histos Harmonia's engine: the API's compositions of historical objects (api/cpp/science/histos.hpp), of tunings
// (science/sounding.hpp) and of argument maps (science/arguments.hpp) behind one program.
//
// It answers the one request given on its input, a JSON object with an "op", with one JSON line: {"ok": true,
// "result": …} or {"ok": false, "error": …}. It reads only the files a request names by their paths (a record, the
// schema's script) and keeps nothing between requests.

#include "science/arguments.hpp"
#include "science/histos.hpp"
#include "science/sounding.hpp"

#include <exception>
#include <iostream>
#include <iterator>
#include <string>

int main() {
  std::ios::sync_with_stdio(false);
  using science::Json;
  const std::string request{std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
  Json reply = Json::object();
  try {
    const Json r = Json::parse(request);
    const std::string op = r["op"].text();
    Json result;
    if (op == "tuning" || op == "compare" || op == "systems") result = science::sounding::answer(r);
    else if (op == "map") result = science::arguments::answer(r);
    else result = science::histos::answer(r);
    reply.set("ok", true);
    reply.set("result", result);
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", false}, {"error", e.what()}});
  }
  std::cout << reply.dump() << "\n";
  return 0;
}
