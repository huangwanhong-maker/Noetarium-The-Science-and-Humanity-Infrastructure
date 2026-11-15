// The Law Studio's engine: the API's composition of the law domain (api/cpp/science/law.hpp) behind the API's program
// (science/program.hpp), a request a line: schemes checked, matters evaluated (evidence, facts, authority, labels, what a
// conclusion rests on, the plan of proof), candidates of evidence recognised. It reads no file.

#include "science/law.hpp"
#include "science/program.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::law::answer); }
