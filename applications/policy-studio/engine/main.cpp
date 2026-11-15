// The Policy Studio's engine: the API's composition of the policy domain (api/cpp/science/policy.hpp) behind the API's program
// (science/program.hpp), a request a line: a theory of change read, a question appraised (bodies of evidence pooled and graded, the
// theory's strongest path, the argument for the place, the options, the decision). It reads no file.

#include "science/policy.hpp"
#include "science/program.hpp"

int main(int argc, char** argv) { return science::program::run(argc, argv, science::policy::answer); }
