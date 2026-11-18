# The risk-management domain's C++: risk as the service a system may lose, and how a loss is financed (ADR 0129).
# Header-only; each module a target risk_management::<module>:
#   criticality   components of a system under the states of a hazard: the expected loss with each failed or hardened, and the
#                 importance measures of probabilistic risk assessment (criticality, risk, Birnbaum, RAW, RRW, Fussell–Vesely)
#   financing     instruments as layers of a loss with their declared costs, strategies compared, a tower chosen, and parametric
#                 cover with its basis risk, on tyche's layers
#
# The domain stands on the foundations alone (module.json): tyche (descriptive: exceedance and layers), calx (verification), and morphe
# and physis as tyche's order requires. Inside Noetarium they are the sibling foundations, added when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(risk_management LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(RISK_MANAGEMENT_TOP_LEVEL ON)
else()
  set(RISK_MANAGEMENT_TOP_LEVEL OFF)
endif()
option(RISK_MANAGEMENT_CHECKS "Build the risk-management domain's checks" ${RISK_MANAGEMENT_TOP_LEVEL})

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
    message(FATAL_ERROR "the risk-management domain needs the foundations: build it inside Noetarium")
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

if(RISK_MANAGEMENT_CHECKS)
  enable_testing()
endif()

function(risk_management_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(risk_management_${name} INTERFACE)
  add_library(risk_management::${name} ALIAS risk_management_${name})
  target_include_directories(risk_management_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(risk_management_${name} INTERFACE cxx_std_20)
  target_link_libraries(risk_management_${name} INTERFACE ${ARG_LIBRARIES})
  if(RISK_MANAGEMENT_CHECKS)
    calx_add_check(check_risk_management_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES risk_management::${name} calx::random)
  endif()
endfunction()

risk_management_module(criticality LIBRARIES tyche::descriptive)
risk_management_module(financing LIBRARIES tyche::descriptive)
