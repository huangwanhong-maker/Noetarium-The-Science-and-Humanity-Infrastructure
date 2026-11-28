// The API's engine of progress among units toward declared targets: a program around science/progress.hpp, answering requests one
// JSON object a line. science.progress builds it into data/cache/engines/api/progress/ and asks it.

#include <science/program.hpp>
#include <science/progress.hpp>

int main(int argc, char** argv) { return science::program::run(argc, argv, [](const science::Json& r) { return science::progress::run(r); }); }
