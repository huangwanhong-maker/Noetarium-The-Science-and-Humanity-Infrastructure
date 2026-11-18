// The plug-in of needs: the API's composition of the needs domain (api/cpp/science/needs.hpp), behind the program the works' engines
// share (api/cpp/science/program.hpp): one JSON request and one answer, or with --serve a request a line. Nothing here names a place:
// the dataset is stated in the request by the plug-in's server side and tools, or by a script of one's own through
// api/python/science/needs.py.

#include "science/needs.hpp"
#include "science/program.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::needs::answer); }
