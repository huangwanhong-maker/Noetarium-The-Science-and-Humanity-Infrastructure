// A program around one of the API's compositions, for any work's engine (ADR 0120, made general by ADR 0121): it answers requests,
// one JSON object a line ({"op": …, …}), with one JSON line each ({"ok": true, "result": …} or {"ok": false, "error": …}); with
// --serve until its input ends, otherwise the one request given on its input or as an argument. A request's "id", where given, is
// returned with its answer. ``science.engines`` keeps such programs running (Engine.keep) or asks them one request a process.
//
//   #include "science/program.hpp"
//   #include "science/law.hpp"
//   int main(int argc, char** argv) { return science::program::run(argc, argv, science::law::answer); }

#pragma once

#include "json.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>

namespace science::program {

template <class Answer>
std::string respond(const std::string& line, Answer answer) {
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

template <class Answer>
int run(int argc, char** argv, Answer answer) {
  std::ios::sync_with_stdio(false);
  bool serve = false;
  std::string request;
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--serve") == 0) serve = true;
    else if (std::strcmp(argv[i], "--data") == 0) i++;  // the data library's products, which a composition may not need
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) continue;
      std::cout << respond(line, answer) << "\n" << std::flush;
    }
    return 0;
  }
  if (request.empty()) request.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
  std::cout << respond(request, answer) << "\n";
  return 0;
}

}  // namespace science::program
