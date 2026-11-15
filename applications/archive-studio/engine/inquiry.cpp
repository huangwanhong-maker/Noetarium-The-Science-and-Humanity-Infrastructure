// The Archive Studio's engine of inquiry: the API's composition of zetesis, tyche, nomos and peira (api/cpp/science/inquiry.hpp)
// behind a program (program.hpp). It reads no file and keeps nothing between requests.

#include "program.hpp"
#include "science/inquiry.hpp"

int main(int argc, char** argv) { return archive_studio::run(argc, argv, science::inquiry::answer); }
