// The engine of the Urban Studio's plug-in "Programmes and public finance" (ADR 0144, ADR 0145): scorecards scored on their ordinal
// levels with their evidence, knowledge and learning programmes read from their needs to their results, and a local government's
// finances (ratios, the years ahead, debt capacity, debt dynamics, capacity and effort). The compositions are the API's
// (science/scorecards.hpp, science/learning.hpp, science/fiscal.hpp); the tools ask this program through science.programmes.
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends.
//
//   scorecard.score · scorecard.compare · scorecard.agreement · learning.assess · fiscal.assess · fiscal.debt · fiscal.effort ·
//   capture.compare · capture.readjust (ADR 0146) · status

#include <science/capture.hpp>
#include <science/fiscal.hpp>
#include <science/json.hpp>
#include <science/learning.hpp>
#include <science/scorecards.hpp>

#include <cstring>
#include <iostream>
#include <string>

namespace {

using science::Json;

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (op.rfind("scorecard.", 0) == 0) return science::scorecards::run(r);
  if (op.rfind("learning.", 0) == 0) return science::learning::run(r);
  if (op.rfind("fiscal.", 0) == 0) return science::fiscal::run(r);
  if (op.rfind("capture.", 0) == 0) return science::capture::run(r);
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
