// What could be built (ADR 0089), without the page: the API's composition (api/cpp/science/design.hpp) answering requests written
// by run.py from the data library's city model.
//
//   study <request.json> <answer.json>
//
// The request is the plug-in's (science/design.hpp names its fields); the answer is written as the plug-in's page would receive it.

#include <science/design.hpp>
#include <science/json.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: study <request.json> <answer.json>\n";
    return 2;
  }
  std::ifstream in(argv[1], std::ios::binary);
  std::ostringstream text;
  text << in.rdbuf();
  try {
    science::design::Designer designer;
    const auto answer = designer.design(science::Json::parse(text.str()));
    std::ofstream(argv[2], std::ios::binary) << answer.dump() << "\n";
  } catch (const std::exception& e) {
    std::ofstream(argv[2], std::ios::binary) << science::Json::object({{"error", e.what()}}).dump() << "\n";
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
