# The urban-risk-management domain's C++: the risks of a city's systems, its roads first (ADR 0129).
# Header-only; each module a target urban_risk_management::<module>:
#   lifelines   a city's roads as links of a system whose service is reaching what people need: the contracted network, people and
#               services joined to it, times under a state of the links, the loss in person-minutes, every link's closure answered at
#               once by the forest of shortest paths, criticality and risk through risk management, redundancy by link-disjoint paths
#   response    coverage by stations within a time under a state of the roads, backup coverage, what each station alone covers,
#               and where to add stations over a hazard's scenarios, greedily with a bound
#
# The domain is a bridge over risk management and urban science (module.json): risk_management::criticality, urban::access (the
# network and its stitching), morphe (shortest paths, flows), calx (greedy submodular maximisation, verification).

cmake_minimum_required(VERSION 3.20)
project(urban_risk_management LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(URBAN_RISK_MANAGEMENT_TOP_LEVEL ON)
else()
  set(URBAN_RISK_MANAGEMENT_TOP_LEVEL OFF)
endif()
option(URBAN_RISK_MANAGEMENT_CHECKS "Build the urban-risk-management domain's checks" ${URBAN_RISK_MANAGEMENT_TOP_LEVEL})

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
    message(FATAL_ERROR "the urban-risk-management domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::topology)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()
if(NOT TARGET physis::units)
  add_subdirectory(${SCIENCE_ROOT}/foundations/physis ${CMAKE_CURRENT_BINARY_DIR}/physis)
endif()
if(NOT TARGET tyche::descriptive)
  add_subdirectory(${SCIENCE_ROOT}/foundations/tyche ${CMAKE_CURRENT_BINARY_DIR}/tyche)
endif()
if(NOT TARGET risk_management::criticality)
  add_subdirectory(${SCIENCE_ROOT}/domains/risk-management ${CMAKE_CURRENT_BINARY_DIR}/risk-management)
endif()
if(NOT TARGET urban::access)
  add_subdirectory(${SCIENCE_ROOT}/domains/urban-science ${CMAKE_CURRENT_BINARY_DIR}/urban-science)
endif()

if(URBAN_RISK_MANAGEMENT_CHECKS)
  enable_testing()
endif()

function(urban_risk_management_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(urban_risk_management_${name} INTERFACE)
  add_library(urban_risk_management::${name} ALIAS urban_risk_management_${name})
  target_include_directories(urban_risk_management_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(urban_risk_management_${name} INTERFACE cxx_std_20)
  target_link_libraries(urban_risk_management_${name} INTERFACE ${ARG_LIBRARIES})
  if(URBAN_RISK_MANAGEMENT_CHECKS)
    calx_add_check(check_urban_risk_management_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES urban_risk_management::${name} calx::random)
  endif()
endfunction()

urban_risk_management_module(lifelines LIBRARIES risk_management::criticality urban::access morphe::combinatorics)
urban_risk_management_module(response LIBRARIES urban_risk_management::lifelines calx::optimisation)
