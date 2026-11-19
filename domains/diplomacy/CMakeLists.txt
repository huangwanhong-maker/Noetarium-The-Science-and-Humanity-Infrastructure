# The diplomacy domain's C++: multilateral relations among parties of every kind (ADR 0132). Header-only; each module a target
# diplomacy::<module>:
#   relations     an undertaking as a typed dependency structure R = (V, {E_k}, D, Γ): participants, ties of declared types in time,
#                 each present, absent or not observed under declared designs of observation; decisions with their authorities,
#                 evidence, those affected and venues of challenge; constraints; a participant's relational position; the
#                 undertaking's own diagnostics (evidence, authorisation, participation, AI dependency, succession by morphe's
#                 blocks, concentration, what is not observed)
#   partnerships  a partnership's record: members, goals, commitments screened by declared criteria (SMART by default), their state
#                 at a date as amended, contributions; a ledger of values by party, dimension and scale, compared as scales permit
#                 and never aggregated without a declared rule; each party's benefit as kybernesis's dual verdict; the modes of
#                 failure a record shows
#
# The domain is a bridge over law and policy (module.json); its first modules stand on the foundations alone: kybernesis (dual
# verdicts, scales), archeion (calendar dates), nomos (concentration), tyche (the normal law), apodeixis (verdicts), morphe
# (blocks of a multigraph), calx (verification, random streams). Inside Noetarium they are the sibling foundations, added here when
# no larger project has added them; the law and policy domains are added when a module first uses them.

cmake_minimum_required(VERSION 3.20)
project(diplomacy LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(DIPLOMACY_TOP_LEVEL ON)
else()
  set(DIPLOMACY_TOP_LEVEL OFF)
endif()
option(DIPLOMACY_CHECKS "Build the diplomacy domain's checks" ${DIPLOMACY_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(diplomacy_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/foundations/${directory}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_ROOT}/foundations/${directory} ${CMAKE_CURRENT_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "the diplomacy domain needs the foundation ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
diplomacy_sibling(calx calx::verification)
diplomacy_sibling(morphe morphe::combinatorics)
diplomacy_sibling(physis physis::units)
diplomacy_sibling(tyche tyche::probability)
diplomacy_sibling(nomos nomos::measurement)
diplomacy_sibling(apodeixis apodeixis::kernel)
diplomacy_sibling(archeion archeion::retention)
diplomacy_sibling(kybernesis kybernesis::kernel)

if(DIPLOMACY_CHECKS)
  enable_testing()
endif()

function(diplomacy_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(diplomacy_${name} INTERFACE)
  add_library(diplomacy::${name} ALIAS diplomacy_${name})
  target_include_directories(diplomacy_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(diplomacy_${name} INTERFACE cxx_std_20)
  target_link_libraries(diplomacy_${name} INTERFACE ${ARG_LIBRARIES})
  if(DIPLOMACY_CHECKS)
    calx_add_check(check_diplomacy_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES diplomacy::${name} calx::random)
  endif()
endfunction()

diplomacy_module(relations LIBRARIES morphe::combinatorics)
diplomacy_module(partnerships LIBRARIES kybernesis::kernel archeion::retention nomos::measurement tyche::probability apodeixis::kernel)
