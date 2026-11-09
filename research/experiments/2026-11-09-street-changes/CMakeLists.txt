# The street-changes study (ADR 0087): urban science's interventions and peira's interference on any city's records.
#
# A standalone project: the urban-science domain is added from the infrastructure by relative path, and it adds the foundations and
# the architecture domain it stands on; the API's JSON is included from api/cpp.
#
#   cmake -S research/experiments/2026-11-09-street-changes -B ~/.cache/science-build/exp-changes -DCMAKE_BUILD_TYPE=Release
#   cmake --build ~/.cache/science-build/exp-changes -j2
#   ~/.cache/science-build/exp-changes/changes <results/<city>-inputs.json> <results/<city>.json> [--bounds 100,400]

cmake_minimum_required(VERSION 3.20)
project(experiment_street_changes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
if(NOT TARGET urban::interventions)
  add_subdirectory(${SCIENCE}/domains/urban-science ${CMAKE_BINARY_DIR}/urban-science)
endif()

add_executable(changes changes.cpp)
target_include_directories(changes PRIVATE ${SCIENCE}/api/cpp)
target_link_libraries(changes PRIVATE urban::interventions)
target_compile_options(changes PRIVATE -Wall -Wextra)
