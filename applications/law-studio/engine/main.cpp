// The Law Studio's engine: the API's composition of the law domain (api/cpp/science/law.hpp), of theories of the case
// (api/cpp/science/theories.hpp) and of theories of the law (api/cpp/science/doctrine.hpp) behind the API's program
// (science/program.hpp), a request a line: schemes checked, matters evaluated (evidence, facts, authority, labels, what a conclusion
// rests on, the plan of proof), candidates of evidence recognised, theories of the case searched for, tested on what is held back
// and read under the law, and doctrines (readings and ratios) searched for by fit and justification. It reads no file.

#include "science/program.hpp"
#include "science/doctrine.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::doctrine::answer); }
