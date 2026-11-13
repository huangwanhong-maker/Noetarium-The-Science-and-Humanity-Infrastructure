# The civil-engineering domain's C++: structures, hydraulics and transport as engineered systems (README §4; founded by ADR 0107).
# Header-only; each module a target civil_engineering::<module>:
#   water       water distribution networks: EPANET's input files read in SI, heads and flows by physis's networks of conduits
#               (Hazen–Williams, Darcy–Weisbach, pumps, check valves, demand- and pressure-driven), a period simulated with tanks and
#               controls, the age of the water, Todini's resilience index, and the criticality of each link

cmake_minimum_required(VERSION 3.20)
project(civil_engineering LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(CIVIL_ENGINEERING_TOP_LEVEL ON)
else()
  set(CIVIL_ENGINEERING_TOP_LEVEL OFF)
endif()
option(CIVIL_ENGINEERING_CHECKS "Build the civil-engineering domain's checks" ${CIVIL_ENGINEERING_TOP_LEVEL})

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
    message(FATAL_ERROR "the civil-engineering domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::topology)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()
if(NOT TARGET physis::units)
  add_subdirectory(${SCIENCE_ROOT}/foundations/physis ${CMAKE_CURRENT_BINARY_DIR}/physis)
endif()

if(CIVIL_ENGINEERING_CHECKS)
  enable_testing()
endif()

function(civil_engineering_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(civil_engineering_${name} INTERFACE)
  add_library(civil_engineering::${name} ALIAS civil_engineering_${name})
  target_include_directories(civil_engineering_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(civil_engineering_${name} INTERFACE cxx_std_20)
  target_link_libraries(civil_engineering_${name} INTERFACE ${ARG_LIBRARIES})
  if(CIVIL_ENGINEERING_CHECKS)
    calx_add_check(check_civil_engineering_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES civil_engineering::${name})
    target_compile_definitions(check_civil_engineering_${name} PRIVATE CIVIL_ENGINEERING_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures")
  endif()
endfunction()

civil_engineering_module(water LIBRARIES physis::discretisation)
