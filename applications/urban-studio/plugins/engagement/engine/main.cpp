// The plug-in of engagements: the API's composition of kybernesis (api/cpp/science/steering.hpp), behind the program the works'
// engines share (api/cpp/science/program.hpp): one JSON request and one answer, or with --serve a request a line. It reads no file:
// a request holds the whole trajectory, which is verified whenever it is read again.

#include "science/program.hpp"
#include "science/steering.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::steering::answer); }
