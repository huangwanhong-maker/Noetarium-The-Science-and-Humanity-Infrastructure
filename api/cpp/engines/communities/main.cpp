// The API's engine of communities (ADR 0167): a program around science/communities.hpp, answering requests one JSON object a line.
// science.communities builds it into data/cache/engines/api/communities/ and asks it.

#include <science/communities.hpp>
#include <science/program.hpp>

int main(int argc, char** argv) { return science::program::run(argc, argv, [](const science::Json& r) { return science::communities::run(r); }); }
