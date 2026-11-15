// The Archive Studio's engine of archives: the API's composition of archeion (api/cpp/science/archive.hpp) behind a program
// (program.hpp): arrangement, custody, access, the journal, packages and datasets. It reads only the files a request names.

#include "program.hpp"
#include "science/archive.hpp"

int main(int argc, char** argv) { return archive_studio::run(argc, argv, science::archive::answer); }
