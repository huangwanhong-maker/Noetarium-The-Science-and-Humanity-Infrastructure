# The architecture domain's C++: buildings (ADR 0060 §2.8, ADR 0062).
# Header-only; each module a target architecture::<module>:
#   space           a building as a complex of rooms: its rooms the 3-cells, walls, floors and facades the 2-cells,
#                   what each room carries; doors and stairs opened, rooms merged and parted, the rooms reached;
#                   a district's buildings as one 3-complex, party walls shared (massing.hpp);
#   environment     the heat of its rooms as the Hodge Laplacian Δ₃ with the fabric in its stars; the Sun's sol-air
#                   gains on its facades and roof; its thermal modes; each hour propagated exactly; the heat of a
#                   district's buildings through their party walls, facades and roofs (massing_heat.hpp); sun and shadow on
#                   the ground and on faces, by thresholds on heights (sun.hpp, ADR 0089);
#   rules           the envelope a lot's setbacks, street wall and limits allow, whether a massing lies within it, and lots
#                   cut by a passage (ADR 0089);
#   form            a massing from a few parameters within the rules, its measures and its prisms for the Sun (ADR 0089);
#   frontage        the street wall: the share of a lot's street line a podium stands on, its height, its active length;
#   plan            a building as it is drawn (ADR 0095): walls on a plane, storey by storey; its rooms as the faces, their net
#                   areas and widths, light and air, the access graph and depths; whether it can be lived in, rule by rule
#                   (habitability.hpp); its quantities, their cost at declared rates and its fabric's heat loss (quantities.hpp);
#   programme       a brief as a labelled graph, and whether a drawn building answers it, statement by statement;
#   interior        what stands in the rooms (ADR 0096): pieces with the clear floor they need; whether each is within its room,
#                   apart from the others, usable and reached by a person of a width, on eikon's distance transform.
#   shell           the outside of a drawn building (ADR 0097): its footprint at the walls' outer faces, its roof (flat, shed, gable,
#                   hip: physis's roof of equal slope) with the walls carried up to it, its faces to the eight winds, and the sun,
#                   the sky and the daylight at its glass (environment's panes.hpp).
# rules also holds the siting of a dwelling on its lot, rule by rule (siting.hpp, ADR 0095).
#
# The domain stands on the foundations alone (module.json): calx (linalg), morphe (topology, formal) and physis
# (discretisation). Inside Noetarium they are the sibling foundations, added here when no larger
# project has added them.

cmake_minimum_required(VERSION 3.20)
project(architecture LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(ARCHITECTURE_TOP_LEVEL ON)
else()
  set(ARCHITECTURE_TOP_LEVEL OFF)
endif()
option(ARCHITECTURE_CHECKS "Build the architecture domain's checks" ${ARCHITECTURE_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
if(NOT TARGET calx::verification)
  if(EXISTS ${SCIENCE_ROOT}/foundations/calx/CMakeLists.txt)
    add_subdirectory(${SCIENCE_ROOT}/foundations/calx ${CMAKE_CURRENT_BINARY_DIR}/calx)
  else()
    message(FATAL_ERROR "the architecture domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::topology)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()
if(NOT TARGET physis::discretisation)
  add_subdirectory(${SCIENCE_ROOT}/foundations/physis ${CMAKE_CURRENT_BINARY_DIR}/physis)
endif()
# syntaxis (ADR 0083): the grammars of geometry and topology, which read footprints and plans.
if(NOT TARGET syntaxis::simplicial)
  add_subdirectory(${SCIENCE_ROOT}/foundations/syntaxis ${CMAKE_CURRENT_BINARY_DIR}/syntaxis)
endif()
# eikon (ADR 0096): the exact distance transform and components of a grid, for whether a piece can be reached.
if(NOT TARGET eikon::distance)
  add_subdirectory(${SCIENCE_ROOT}/foundations/eikon ${CMAKE_CURRENT_BINARY_DIR}/eikon)
endif()
if(ARCHITECTURE_CHECKS)
  enable_testing()
endif()

function(architecture_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(architecture_${name} INTERFACE)
  add_library(architecture::${name} ALIAS architecture_${name})
  target_include_directories(architecture_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(architecture_${name} INTERFACE cxx_std_20)
  target_link_libraries(architecture_${name} INTERFACE ${ARG_LIBRARIES})
  if(ARCHITECTURE_CHECKS)
    calx_add_check(check_architecture_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES architecture::${name})
  endif()
endfunction()
function(architecture_check module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  if(ARCHITECTURE_CHECKS)
    calx_add_check(check_architecture_${name} SOURCES ${module}/tests/check_${name}.cpp LIBRARIES architecture::${module} ${ARG_LIBRARIES})
  endif()
endfunction()

architecture_module(space LIBRARIES morphe::topology morphe::formal)
architecture_module(environment LIBRARIES architecture::space physis::discretisation physis::computational_geometry calx::linalg calx::integrators)
architecture_check(space massing)
architecture_check(environment sun LIBRARIES calx::random)
architecture_check(environment balance)
architecture_module(rules LIBRARIES physis::computational_geometry)
architecture_module(form LIBRARIES architecture::rules architecture::environment)
architecture_module(frontage LIBRARIES architecture::rules architecture::form)
architecture_module(syntax LIBRARIES syntaxis::induction syntaxis::simplicial)
architecture_check(rules siting)
architecture_module(plan LIBRARIES architecture::rules physis::computational_geometry morphe::combinatorics)
architecture_check(plan quantities)
architecture_check(plan habitability)
architecture_module(programme LIBRARIES architecture::plan)
architecture_module(interior LIBRARIES architecture::plan eikon::distance eikon::segmentation)
architecture_check(environment panes)
architecture_check(environment solar)
architecture_module(shell LIBRARIES architecture::plan architecture::environment physis::computational_geometry)
