# The demography domain's C++: populations by age, their deaths, births and growth (ADR 0085).
# Header-only; each module a target demography::<module>:
#   lifetable       the period life table from death rates (a_x by default or exact for a constant hazard), rates read from
#                   peira's Lexis diagram, death rates standardised through peira, Arriaga's decomposition of e_0
#   fertility       the total, gross and net reproduction rates and the mean ages at childbearing
#   projection      the Leslie matrix, projection, the stable population (λ, r, its structure, R₀) and Lotka's r
#
# The domain stands on the foundations alone (module.json): peira (cohorts: the Lexis diagram, rates, standardisation), tyche
# (probability, through peira), calx (solvers, verification) and morphe and physis as the siblings' order requires. Inside
# Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(demography LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(DEMOGRAPHY_TOP_LEVEL ON)
else()
  set(DEMOGRAPHY_TOP_LEVEL OFF)
endif()
option(DEMOGRAPHY_CHECKS "Build the demography domain's checks" ${DEMOGRAPHY_TOP_LEVEL})

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
    message(FATAL_ERROR "the demography domain needs the foundations: build it inside Noetarium")
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

if(DEMOGRAPHY_CHECKS)
  enable_testing()
endif()

function(demography_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(demography_${name} INTERFACE)
  add_library(demography::${name} ALIAS demography_${name})
  target_include_directories(demography_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(demography_${name} INTERFACE cxx_std_20)
  target_link_libraries(demography_${name} INTERFACE ${ARG_LIBRARIES})
  if(DEMOGRAPHY_CHECKS)
    calx_add_check(check_demography_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES demography::${name} calx::random)
  endif()
endfunction()

demography_module(lifetable LIBRARIES peira::cohorts tyche::probability)
demography_module(fertility LIBRARIES demography::lifetable)
demography_module(projection LIBRARIES demography::lifetable demography::fertility calx::solvers)
demography_module(health LIBRARIES demography::lifetable calx::linalg)
