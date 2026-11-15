// What the Archive Studio's engines share: a program that answers requests, one JSON object a line ({"op": …, …}), with one JSON
// line each ({"ok": true, "result": …} or {"ok": false, "error": …}); with --serve until its input ends, otherwise the one request
// given on its input or as an argument. Each engine names the API composition that answers (linked data, archives, inquiry).

#pragma once

#include "science/json.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>

namespace archive_studio {

template <class Answer>
std::string respond(const std::string& line, Answer answer) {
  science::Json reply = science::Json::object();
  try {
    const science::Json r = science::Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", answer(r));
  } catch (const std::exception& e) {
    reply = science::Json::object({{"ok", false}, {"error", e.what()}});
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
    else if (std::strcmp(argv[i], "--data") == 0) i++;  // the data library's products, which these engines never read
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

}  // namespace archive_studio
