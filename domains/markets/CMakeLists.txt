# The markets domain's C++: buyers, offers and the kernel between them, and what is computed from them (ADR 0092).
# Header-only; each module a target markets::<module>:
#   market     a market M = (B, μ, q; O, o₀; K): demand, shares and structure, trade areas as densities with their cores and overlaps,
#              unserved spend, segments as partitions with what they lose, and the logit as a choice rule with its diversion
#   sizing     the size of a market: trees of sums and products with exact bounds, elasticities and swings; TAM, SAM, SOM; two
#              estimates reconciled; buyers of a later year on demography's projection; derived demand on economics' input–output table
#   space      Huff's model as that logit, Reilly's law, the nearest outlet, the reach of an outlet, a new outlet and whom it draws from,
#              and where to open, greedily, with its bound
#
# The domain is a bridge over economics (production) and demography (projection), on nomos (measurement) and calx (module.json).
# Inside Noetarium they are the sibling foundations and domains, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(markets LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(MARKETS_TOP_LEVEL ON)
else()
  set(MARKETS_TOP_LEVEL OFF)
endif()
option(MARKETS_CHECKS "Build the markets domain's checks" ${MARKETS_TOP_LEVEL})

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
    message(FATAL_ERROR "the markets domain needs the foundations: build it inside Noetarium")
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
if(NOT TARGET peira::cohorts)
  add_subdirectory(${SCIENCE_ROOT}/foundations/peira ${CMAKE_CURRENT_BINARY_DIR}/peira)
endif()
if(NOT TARGET nomos::measurement)
  add_subdirectory(${SCIENCE_ROOT}/foundations/nomos ${CMAKE_CURRENT_BINARY_DIR}/nomos)
endif()
if(NOT TARGET economics::production)
  add_subdirectory(${SCIENCE_ROOT}/domains/economics ${CMAKE_CURRENT_BINARY_DIR}/economics)
endif()
if(NOT TARGET demography::projection)
  add_subdirectory(${SCIENCE_ROOT}/domains/demography ${CMAKE_CURRENT_BINARY_DIR}/demography)
endif()

if(MARKETS_CHECKS)
  enable_testing()
endif()

function(markets_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(markets_${name} INTERFACE)
  add_library(markets::${name} ALIAS markets_${name})
  target_include_directories(markets_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(markets_${name} INTERFACE cxx_std_20)
  target_link_libraries(markets_${name} INTERFACE ${ARG_LIBRARIES})
  if(MARKETS_CHECKS)
    calx_add_check(check_markets_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES markets::${name} calx::random)
  endif()
endfunction()

markets_module(market LIBRARIES nomos::measurement)
markets_module(sizing LIBRARIES demography::projection economics::production)
markets_module(space LIBRARIES markets::market nomos::measurement)
