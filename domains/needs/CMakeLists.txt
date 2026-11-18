# The needs domain's C++: the evolutionary exploration and validation of needs (ADR 0125). Header-only; each module a target
# needs::<module>:
#   groups     tables of units, and groups of them stated as conjunctions of conditions on their attributes
#   language   statements of need (a group, a domain, an indicator, one of Bradshaw's four standards), their grammar in BNF read by
#              morphe, the dataset a statement is judged on
#   evidence   the three parts of the data; the gap with its interval by design (peira's surveys) or by binomial reading (tyche);
#              stability under resampling of clusters; causal support on a declared graph (peira, tyche); simulation of seeking
#              (nomos); rules by forward chaining (morphe); standing by an argumentation framework (apodeixis); a statement read
#              under the readings of its open terms, causal support under each graph, and rivals (tekmerion's construal, ADR 0126)
#   search     the space of statements, generators without a language model, operators of variation, and the search: heuresis's
#              evolution with every stage's strategy chosen, admission through a reusable holdout and confirmation in order by an
#              online procedure (tyche), each test sealed in archeion's journal before it runs
#
# The domain stands on the foundations alone (module.json). Inside Noetarium they are the sibling foundations, added here when no
# larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(needs LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(NEEDS_TOP_LEVEL ON)
else()
  set(NEEDS_TOP_LEVEL OFF)
endif()
option(NEEDS_CHECKS "Build the needs domain's checks" ${NEEDS_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(needs_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/foundations/${directory}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_ROOT}/foundations/${directory} ${CMAKE_CURRENT_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "the needs domain needs the foundation ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
needs_sibling(calx calx::verification)
needs_sibling(morphe morphe::formal)
needs_sibling(physis physis::units)
needs_sibling(tyche tyche::testing)
needs_sibling(peira peira::surveys)
needs_sibling(nomos nomos::measurement)
needs_sibling(heuresis heuresis::evolution)
needs_sibling(apodeixis apodeixis::argumentation)
needs_sibling(archeion archeion::fixity)
needs_sibling(tekmerion tekmerion::construal)

if(NEEDS_CHECKS)
  enable_testing()
endif()

function(needs_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(needs_${name} INTERFACE)
  add_library(needs::${name} ALIAS needs_${name})
  target_include_directories(needs_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(needs_${name} INTERFACE cxx_std_20)
  target_link_libraries(needs_${name} INTERFACE ${ARG_LIBRARIES})
  if(NEEDS_CHECKS)
    calx_add_check(check_needs_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES needs::${name} calx::random)
  endif()
endfunction()

needs_module(groups)
needs_module(language LIBRARIES needs::groups morphe::formal)
needs_module(evidence LIBRARIES needs::language peira::surveys peira::graphs tyche::testing tyche::regression tyche::probability
             nomos::measurement nomos::agents morphe::combinatorics apodeixis::argumentation tekmerion::construal calx::random)
needs_module(search LIBRARIES needs::evidence heuresis::evolution archeion::fixity tyche::testing)
