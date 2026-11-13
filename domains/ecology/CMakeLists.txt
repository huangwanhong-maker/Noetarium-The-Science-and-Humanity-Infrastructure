# The ecology domain's C++: organisms among each other and in their environment (ADR 0061, founded with code by ADR 0101).
# Header-only; each module a target ecology::<module>:
#   taxonomy       individuals named at the ranks of a classification (species, genus, family); tallies at a rank; taxonomic
#                  distinctness (Warwick and Clarke) and its expectation under random draws from a master list
#   biodiversity   an assemblage's diversity at each rank on tyche's classes: Hill profiles, estimated and standardised by
#                  coverage, partitioned among areas
#   populations    stage-classified populations (Lefkovitch matrices) from vital rates: λ, the stable structure, reproductive
#                  values, sensitivities and elasticities to matrix entries and to vital rates (calx's non-negative matrices),
#                  life expectancy by stage, projection with an input (planting, immigration), its equilibrium, and
#                  demographic stochasticity
#   forest         a forest of individual trees (urban forests first): allometry as declared schemes (biomass, roots, carbon;
#                  height and crown where a scheme states them), diameter increments from one inventory, composition against
#                  declared limits by rank (Santamour's 10-20-30 as one scheme), size structure, crowns on a grid, the forest's
#                  future under removals and planting, and the stage model that a forest's rates imply
#   connectivity   habitat as a network: the integral index and the probability of connectivity with the importance of each
#                  patch split into its intra, flux and connector parts; least-cost distances on a resistance surface (morphe's
#                  Dijkstra) and circuit theory's effective resistances and current (physis's Kirchhoff networks)
#
# The domain declares geography, climate and evolution (module.json), whose C++ it does not yet use, and stands here on the
# foundations: calx (linalg, random, verification), morphe (combinatorics), physis (discretisation), tyche (classes) and
# peira. Inside Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(ecology LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(ECOLOGY_TOP_LEVEL ON)
else()
  set(ECOLOGY_TOP_LEVEL OFF)
endif()
option(ECOLOGY_CHECKS "Build the ecology domain's checks" ${ECOLOGY_TOP_LEVEL})

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
    message(FATAL_ERROR "the ecology domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::topology)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()
if(NOT TARGET physis::units)
  add_subdirectory(${SCIENCE_ROOT}/foundations/physis ${CMAKE_CURRENT_BINARY_DIR}/physis)
endif()
if(NOT TARGET tyche::probability)
  add_subdirectory(${SCIENCE_ROOT}/foundations/tyche ${CMAKE_CURRENT_BINARY_DIR}/tyche)
endif()
if(NOT TARGET peira::weighting)
  add_subdirectory(${SCIENCE_ROOT}/foundations/peira ${CMAKE_CURRENT_BINARY_DIR}/peira)
endif()

if(ECOLOGY_CHECKS)
  enable_testing()
endif()

function(ecology_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(ecology_${name} INTERFACE)
  add_library(ecology::${name} ALIAS ecology_${name})
  target_include_directories(ecology_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(ecology_${name} INTERFACE cxx_std_20)
  target_link_libraries(ecology_${name} INTERFACE ${ARG_LIBRARIES})
  if(ECOLOGY_CHECKS)
    calx_add_check(check_ecology_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES ecology::${name} calx::random)
  endif()
endfunction()

ecology_module(taxonomy LIBRARIES calx::random)
ecology_module(biodiversity LIBRARIES ecology::taxonomy tyche::classes)
ecology_module(populations LIBRARIES calx::linalg calx::random)
ecology_module(forest LIBRARIES ecology::taxonomy ecology::populations calx::random)
ecology_module(connectivity LIBRARIES calx::linalg morphe::combinatorics physis::discretisation)
ecology_module(ecosystems)
