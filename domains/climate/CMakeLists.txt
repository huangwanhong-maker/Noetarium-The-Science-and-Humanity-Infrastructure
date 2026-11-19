# The climate domain's C++: the planet's energy balance with its ice–albedo feedback (energy_balance) and the overturning of an
# ocean basin in two boxes (overturning), the first models of the domain with folds in nature (ADR 0135). Header-only; each
# module a target climate::<module>. The classification of climates is provisional JavaScript (js/).
#
# The domain uses calx and physis, and kinesis (with morphe and tyche beneath it) in its checks, which read the models'
# records with kinesis's detectors of critical transitions. Inside Noetarium they are the sibling foundations, added here
# when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(climate LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(CLIMATE_TOP_LEVEL ON)
else()
  set(CLIMATE_TOP_LEVEL OFF)
endif()
option(CLIMATE_CHECKS "Build the climate domain's checks" ${CLIMATE_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_FOUNDATIONS ${CMAKE_CURRENT_SOURCE_DIR}/../../foundations)
foreach(pair "calx;calx::verification" "morphe;morphe::topology" "physis;physis::dynamics" "tyche;tyche::probability" "kinesis;kinesis::kernel")
  list(GET pair 0 library)
  list(GET pair 1 target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_FOUNDATIONS}/${library}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_FOUNDATIONS}/${library} ${CMAKE_CURRENT_BINARY_DIR}/${library})
    else()
      message(FATAL_ERROR "the climate domain needs ${library}: build it inside Noetarium")
    endif()
  endif()
endforeach()
if(CLIMATE_CHECKS)
  enable_testing()
endif()

foreach(module energy_balance overturning)
  add_library(climate_${module} INTERFACE)
  add_library(climate::${module} ALIAS climate_${module})
  target_include_directories(climate_${module} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${module}/include)
  target_compile_features(climate_${module} INTERFACE cxx_std_20)
  if(CLIMATE_CHECKS)
    calx_add_check(check_climate_${module} SOURCES ${module}/tests/check_${module}.cpp LIBRARIES climate::${module} kinesis::transitions)
  endif()
endforeach()
