# The philosophy domain's C++: questions, the positions taken on them and the arguments for and against.
# Header-only; each module a target philosophy::<module>:
#   arguments       abstract argumentation after Dung (frameworks, the grounded extension as a least fixed point, complete,
#                   preferred and stable extensions, labellings, acceptance; supports by the attacks they induce), and argument
#                   maps after IBIS (questions, positions, arguments; their faults, the framework a map induces, each position's
#                   standing, the open questions, a written form)
#
# The domain stands on the foundations alone (module.json): morphe (combinatorics: directed graphs, shortest paths) and calx
# (verification). Inside Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(philosophy LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(PHILOSOPHY_TOP_LEVEL ON)
else()
  set(PHILOSOPHY_TOP_LEVEL OFF)
endif()
option(PHILOSOPHY_CHECKS "Build the philosophy domain's checks" ${PHILOSOPHY_TOP_LEVEL})

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
    message(FATAL_ERROR "the philosophy domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::combinatorics)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()

if(PHILOSOPHY_CHECKS)
  enable_testing()
endif()

function(philosophy_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(philosophy_${name} INTERFACE)
  add_library(philosophy::${name} ALIAS philosophy_${name})
  target_include_directories(philosophy_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(philosophy_${name} INTERFACE cxx_std_20)
  target_link_libraries(philosophy_${name} INTERFACE ${ARG_LIBRARIES})
  if(PHILOSOPHY_CHECKS)
    calx_add_check(check_philosophy_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES philosophy::${name} calx::random)
  endif()
endfunction()

philosophy_module(arguments LIBRARIES morphe::combinatorics)
