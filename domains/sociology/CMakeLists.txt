# The sociology domain's C++: groups, institutions, networks and the organisation of everyday life (README §4; founded with code by ADR 0104).
# Header-only; each module a target sociology::<module>:
#   time        a day as a composition of activities on tyche's compositions: activities grouped by a declared classification, the
#               centre and spread of a group's day, differences between groups in Aitchison's geometry, who carries which work, and what
#               moving time from one activity to another would do
#   relations   ties among people on tyche's statistics of relational data: closure, reciprocity, homophily, brokerage, and the
#               association of networks under relational dependence

cmake_minimum_required(VERSION 3.20)
project(sociology LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(SOCIOLOGY_TOP_LEVEL ON)
else()
  set(SOCIOLOGY_TOP_LEVEL OFF)
endif()
option(SOCIOLOGY_CHECKS "Build the sociology domain's checks" ${SOCIOLOGY_TOP_LEVEL})

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
    message(FATAL_ERROR "the sociology domain needs the foundations: build it inside Noetarium")
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
if(NOT TARGET nomos::measurement)
  add_subdirectory(${SCIENCE_ROOT}/foundations/nomos ${CMAKE_CURRENT_BINARY_DIR}/nomos)
endif()

if(SOCIOLOGY_CHECKS)
  enable_testing()
endif()

function(sociology_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(sociology_${name} INTERFACE)
  add_library(sociology::${name} ALIAS sociology_${name})
  target_include_directories(sociology_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(sociology_${name} INTERFACE cxx_std_20)
  target_link_libraries(sociology_${name} INTERFACE ${ARG_LIBRARIES})
  if(SOCIOLOGY_CHECKS)
    calx_add_check(check_sociology_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES sociology::${name} calx::random)
    target_compile_definitions(check_sociology_${name} PRIVATE SOCIOLOGY_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures")
  endif()
endfunction()

sociology_module(time LIBRARIES tyche::compositions tyche::multivariate)
sociology_module(relations LIBRARIES tyche::networks tyche::classes)
