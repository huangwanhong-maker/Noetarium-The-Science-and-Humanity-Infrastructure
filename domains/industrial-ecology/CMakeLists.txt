# The industrial-ecology domain's C++: the material and energy stocks and flows of a society (README §4; founded by ADR 0106).
# Header-only; each module a target industrial_ecology::<module>:
#   stocks      goods in use: lifetime laws (tyche, peira), dynamic stock models driven by inflows or by stocks, the materials they hold
#   flows       material flow analysis: systems of processes and flows, transfer coefficients, substances, the reconciliation of measured flows
#   lca         life cycle assessment by the matrix method: multifunctional processes, impacts, sensitivities, Monte Carlo, structural paths
#   eeio        environmentally extended input–output on economics' Leontief model: footprints, regions, trade, supply chains
#   accounts    physical supply–use tables and their identities; economy-wide material flow indicators
#   buildings   the material stock of buildings: declared intensities with bounds, stocks by group, cohorts by age

cmake_minimum_required(VERSION 3.20)
project(industrial_ecology LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(INDUSTRIAL_ECOLOGY_TOP_LEVEL ON)
else()
  set(INDUSTRIAL_ECOLOGY_TOP_LEVEL OFF)
endif()
option(INDUSTRIAL_ECOLOGY_CHECKS "Build the industrial-ecology domain's checks" ${INDUSTRIAL_ECOLOGY_TOP_LEVEL})

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
    message(FATAL_ERROR "the industrial-ecology domain needs the foundations: build it inside Noetarium")
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
if(NOT TARGET nomos::measurement)
  add_subdirectory(${SCIENCE_ROOT}/foundations/nomos ${CMAKE_CURRENT_BINARY_DIR}/nomos)
endif()
if(NOT TARGET economics::production)
  add_subdirectory(${SCIENCE_ROOT}/domains/economics ${CMAKE_CURRENT_BINARY_DIR}/economics)
endif()

if(INDUSTRIAL_ECOLOGY_CHECKS)
  enable_testing()
endif()

function(industrial_ecology_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(industrial_ecology_${name} INTERFACE)
  add_library(industrial_ecology::${name} ALIAS industrial_ecology_${name})
  target_include_directories(industrial_ecology_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(industrial_ecology_${name} INTERFACE cxx_std_20)
  target_link_libraries(industrial_ecology_${name} INTERFACE ${ARG_LIBRARIES})
  if(INDUSTRIAL_ECOLOGY_CHECKS)
    calx_add_check(check_industrial_ecology_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES industrial_ecology::${name} calx::random)
    target_compile_definitions(check_industrial_ecology_${name} PRIVATE INDUSTRIAL_ECOLOGY_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures")
  endif()
endfunction()

industrial_ecology_module(stocks LIBRARIES tyche::probability peira::survival)
industrial_ecology_module(flows LIBRARIES calx::linalg tyche::probability)
industrial_ecology_module(lca LIBRARIES calx::linalg calx::random)
industrial_ecology_module(eeio LIBRARIES economics::production calx::linalg)
industrial_ecology_module(accounts LIBRARIES calx::linalg)
industrial_ecology_module(buildings LIBRARIES industrial_ecology::stocks)
