// The Archive Studio's engine of linked data: the API's composition of symploke (api/cpp/science/linked.hpp) behind a program
// (program.hpp). It reads no file and keeps nothing between requests: the documents a request holds are all it is told.

#include "program.hpp"
#include "science/linked.hpp"

int main(int argc, char** argv) { return archive_studio::run(argc, argv, science::linked::answer); }
