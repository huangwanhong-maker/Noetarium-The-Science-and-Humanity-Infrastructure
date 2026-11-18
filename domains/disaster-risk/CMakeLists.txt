# The disaster-risk domain's C++: what a hazard does to a city's systems, and getting its people to safety (ADR 0129).
# Header-only; each module a target disaster_risk::<module>:
#   disruption   a hazard's depth read along each link of the roads, depth-disruption curves (cars, Pregnolato et al. 2017; on foot,
#                declared), the local frame, and the scenarios of maps at return periods weighted to integrate over the curve
#   evacuation   evacuation on foot: the walking network, reach within a warning time with and without capacities, the bound by flows
#                over time, the crowd simulated as kinematic waves with routes to the nearest shelter with room, shelters chosen
#                greedily with a bound
#
# The domain stands on urban risk management (module.json), and through it on risk management and urban science; it uses urban
# science's exposure grids, morphe's shortest paths and flows over time, physis's kinematic waves and calx's greedy choice.

cmake_minimum_required(VERSION 3.20)
project(disaster_risk LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(DISASTER_RISK_TOP_LEVEL ON)
else()
  set(DISASTER_RISK_TOP_LEVEL OFF)
endif()
option(DISASTER_RISK_CHECKS "Build the disaster-risk domain's checks" ${DISASTER_RISK_TOP_LEVEL})

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
    message(FATAL_ERROR "the disaster-risk domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::topology)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()
if(NOT TARGET physis::continuum)
  add_subdirectory(${SCIENCE_ROOT}/foundations/physis ${CMAKE_CURRENT_BINARY_DIR}/physis)
endif()
if(NOT TARGET tyche::descriptive)
  add_subdirectory(${SCIENCE_ROOT}/foundations/tyche ${CMAKE_CURRENT_BINARY_DIR}/tyche)
endif()
if(NOT TARGET urban_risk_management::lifelines)
  add_subdirectory(${SCIENCE_ROOT}/domains/urban-risk-management ${CMAKE_CURRENT_BINARY_DIR}/urban-risk-management)
endif()

if(DISASTER_RISK_CHECKS)
  enable_testing()
endif()

function(disaster_risk_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(disaster_risk_${name} INTERFACE)
  add_library(disaster_risk::${name} ALIAS disaster_risk_${name})
  target_include_directories(disaster_risk_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(disaster_risk_${name} INTERFACE cxx_std_20)
  target_link_libraries(disaster_risk_${name} INTERFACE ${ARG_LIBRARIES})
  if(DISASTER_RISK_CHECKS)
    calx_add_check(check_disaster_risk_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES disaster_risk::${name} calx::random)
  endif()
endfunction()

disaster_risk_module(disruption LIBRARIES urban_risk_management::lifelines urban::exposure)
disaster_risk_module(evacuation LIBRARIES urban_risk_management::lifelines physis::continuum calx::optimisation)
