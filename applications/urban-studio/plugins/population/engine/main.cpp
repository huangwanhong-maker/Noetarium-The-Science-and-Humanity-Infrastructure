// The plug-in of population dynamics: the API's composition of kinesis's fields (api/cpp/science/population.hpp), behind the
// program the works' engines share (api/cpp/science/program.hpp): one JSON request and one answer, or with --serve a request a
// line. Nothing here names a place: the field is stated in the request by the plug-in's server side and tools, or by a script of
// one's own through api/python/science/population.py.

#include "science/population.hpp"
#include "science/program.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::population::answer); }
