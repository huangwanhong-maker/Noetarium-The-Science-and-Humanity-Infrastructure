# The allocation domain's C++: what is allocated when there is not enough, to whom first, what and how much, and by what
# procedure (ADR 0136). Header-only; each module a target allocation::<module>:
#   rounds      an allocation round: a resource short of what is wanted, the entities that want it, and the stages that divide it,
#               each serving a declared principle, declared by someone for a stated reason; who was reached first, and why
#   verdicts    a round judged against its alternatives (other rules, other orders) under declared principles at once: the
#               undominated options, the pivots, where a parameter flips a comparison, what is at stake for each entity
#   procedure   the round's record in archeion's sealed journal: published, applied, appealed, heard and revised for everyone;
#               lotteries from committed seeds; the checkable rules of procedural justice (Leventhal, accountability for
#               reasonableness)
#   incidence   who finally gains: re-spending and LM3, capitalisation into rents with exact intervals, the return to generators
#               on a network of flows, floor and ceiling, and targeting
#   evolution   heterogeneous allocation searched by evolution: resources whose types kategoria verifies, carried by heuresis's
#               spaces, granted in an order heuresis's sequential reads, assessed by evaluators of many kinds, every allocation's
#               lawfulness a verified constraint, the kept compared under principles, the search sealed (ADR 0137)
#
# A field at height 1 on economics (module.json): nomos (allocation, welfare), archeion (fixity) and apodeixis (verdicts), the
# economics domain's circulation, and calx, morphe, physis and tyche as their order requires. Inside Noetarium they are the sibling
# foundations and the economics domain, added when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(allocation LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(ALLOCATION_TOP_LEVEL ON)
else()
  set(ALLOCATION_TOP_LEVEL OFF)
endif()
option(ALLOCATION_CHECKS "Build the allocation domain's checks" ${ALLOCATION_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(allocation_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/${directory}/CMakeLists.txt)
      get_filename_component(leaf ${directory} NAME)
      add_subdirectory(${SCIENCE_ROOT}/${directory} ${CMAKE_CURRENT_BINARY_DIR}/${leaf})
    else()
      message(FATAL_ERROR "the allocation domain needs ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
allocation_sibling(foundations/calx calx::verification)
allocation_sibling(foundations/morphe morphe::topology)
allocation_sibling(foundations/physis physis::discretisation)
allocation_sibling(foundations/tyche tyche::descriptive)
allocation_sibling(foundations/nomos nomos::allocation)
allocation_sibling(foundations/apodeixis apodeixis::kernel)
allocation_sibling(foundations/archeion archeion::fixity)
allocation_sibling(foundations/heuresis heuresis::assessment)
allocation_sibling(foundations/kategoria kategoria::resources)
allocation_sibling(foundations/kinesis kinesis::kernel)
allocation_sibling(domains/economics economics::circulation)

if(ALLOCATION_CHECKS)
  enable_testing()
endif()

function(allocation_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(allocation_${name} INTERFACE)
  add_library(allocation::${name} ALIAS allocation_${name})
  target_include_directories(allocation_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(allocation_${name} INTERFACE cxx_std_20)
  target_link_libraries(allocation_${name} INTERFACE ${ARG_LIBRARIES})
  if(ALLOCATION_CHECKS)
    calx_add_check(check_allocation_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES allocation::${name} calx::random)
  endif()
endfunction()

allocation_module(rounds LIBRARIES nomos::allocation)
allocation_module(verdicts LIBRARIES allocation::rounds nomos::welfare)
allocation_module(procedure LIBRARIES allocation::rounds archeion::fixity apodeixis::kernel)
allocation_module(incidence LIBRARIES economics::circulation)
allocation_module(evolution LIBRARIES allocation::rounds nomos::welfare archeion::fixity heuresis::assessment heuresis::sequential heuresis::spaces kategoria::resources calx::optimisation)
allocation_module(evaluators LIBRARIES allocation::evolution economics::ledger kinesis::densities kinesis::induction)
if(ALLOCATION_CHECKS)
  # The composition on the OR-Library's knapsacks (ADR 0141), read from the data library's cache where it holds them.
  calx_add_check(check_allocation_orlib SOURCES evolution/tests/check_orlib.cpp LIBRARIES allocation::evolution heuresis::benchmarks calx::random)
  calx_add_check(check_allocation_budget SOURCES evolution/tests/check_budget.cpp LIBRARIES allocation::evolution calx::random)
  calx_add_check(check_allocation_policies SOURCES evolution/tests/check_policies.cpp LIBRARIES allocation::evolution calx::random)
  target_compile_definitions(check_allocation_orlib PRIVATE ALLOCATION_CACHE="${SCIENCE_ROOT}/data/cache")
endif()
