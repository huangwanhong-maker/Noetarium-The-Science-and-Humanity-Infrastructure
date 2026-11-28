// The API's engine of a community's readings (Phase 9): a program around science/community_readings.hpp, the communities domain's
// trajectories (measures of power, value readings), answering requests one JSON object a line. science.communities builds it into
// data/cache/engines/api/community-readings/ and asks it.

#include <science/community_readings.hpp>
#include <science/program.hpp>

int main(int argc, char** argv) { return science::program::run(argc, argv, [](const science::Json& r) { return science::communities::readings::run(r); }); }
