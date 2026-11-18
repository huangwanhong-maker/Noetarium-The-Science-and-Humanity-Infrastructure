// The plug-in of cities across scales: the API's composition of kinesis's systems across scales, tyche's estimates and
// change of support, and nomos's measures as reducers (api/cpp/science/scales.hpp), behind the program the works' engines share
// (api/cpp/science/program.hpp): one JSON request and one answer, or with --serve a request a line. Nothing here names a place:
// the hierarchy, the model and its inputs are stated in the request by the plug-in's server side and tools, or by a script of
// one's own through api/python/science/scales.py.

#include "science/program.hpp"
#include "science/scales.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::scales::answer); }
