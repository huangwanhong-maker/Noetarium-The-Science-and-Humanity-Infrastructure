// The plug-in of evidence in space and time: the API's composition of archeion, tekmerion and tyche with geodesy's distances
// (api/cpp/science/spacetime_evidence.hpp), behind the program the works' engines share (api/cpp/science/program.hpp): one JSON
// request and one answer, or with --serve a request a line. Nothing here names a place: the holdings, the items, the sources, the
// claims and the decision are stated in the request by the plug-in's server side and tools, or by a script of one's own through
// api/python/science/spacetime_evidence.py.

#include "science/program.hpp"
#include "science/spacetime_evidence.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::spacetime_evidence::answer); }
