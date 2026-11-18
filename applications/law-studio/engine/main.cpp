// The Law Studio's engine: the API's composition of the law domain (api/cpp/science/law.hpp) and of theories of the case
// (api/cpp/science/theories.hpp) behind the API's program (science/program.hpp), a request a line: schemes checked, matters
// evaluated (evidence, facts, authority, labels, what a conclusion rests on, the plan of proof), candidates of evidence
// recognised, and theories of the case searched for, tested on what is held back and read under the law. It reads no file.

#include "science/program.hpp"
#include "science/theories.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::theories::answer); }
