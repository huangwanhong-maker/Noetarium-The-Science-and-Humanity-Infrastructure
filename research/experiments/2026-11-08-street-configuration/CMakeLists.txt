# The configuration of Melbourne's streets against its walkers (ADR 0084): urban science's configuration (Space Syntax's integration
# and choice on morphe's centralities) computed on the city model's streets, at each cost and radius, read at each pedestrian counter.
#
# A standalone project: the urban-science domain is added from the infrastructure by relative path, and it adds the foundations and
# the architecture domain it stands on; the API's JSON is included from api/cpp.
#
#   cmake -S research/experiments/2026-11-08-street-configuration -B ~/.cache/science-build/exp-configuration -DCMAKE_BUILD_TYPE=Release
#   cmake --build ~/.cache/science-build/exp-configuration -j2
#   ~/.cache/science-build/exp-configuration/configuration <results/inputs.json> <results directory> [--threads N]

cmake_minimum_required(VERSION 3.20)
project(experiment_street_configuration LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()
find_package(Threads REQUIRED)

set(SCIENCE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
if(NOT TARGET urban::syntax)
  add_subdirectory(${SCIENCE}/domains/urban-science ${CMAKE_BINARY_DIR}/urban-science)
endif()

add_executable(configuration configuration.cpp)
target_include_directories(configuration PRIVATE ${SCIENCE}/api/cpp)
target_link_libraries(configuration PRIVATE urban::syntax Threads::Threads)
target_compile_options(configuration PRIVATE -Wall -Wextra)
