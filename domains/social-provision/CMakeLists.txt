# The social-provision domain's C++: how what sustains life reaches people, in households, by care, public services and commons,
# outside markets as well as through them (ADR 0105). A bridge over economics, markets, demography and sociology.
# Header-only; each module a target social_provision::<module>:
#   households   equivalence scales as declared schemes; the value of unpaid work by replacement and opportunity cost; full income;
#                time poverty against a declared line
#   care         people needing care from population and prevalence, with scenarios; supply, informal and formal; the gap; the care
#                diamond; support ratios; a change of need split exactly among size, age structure and prevalence (nomos's Shapley)

cmake_minimum_required(VERSION 3.20)
project(social_provision LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(SOCIAL_PROVISION_TOP_LEVEL ON)
else()
  set(SOCIAL_PROVISION_TOP_LEVEL OFF)
endif()
option(SOCIAL_PROVISION_CHECKS "Build the social-provision domain's checks" ${SOCIAL_PROVISION_TOP_LEVEL})

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
    message(FATAL_ERROR "the social-provision domain needs the foundations: build it inside Noetarium")
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

if(SOCIAL_PROVISION_CHECKS)
  enable_testing()
endif()

function(social_provision_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(social_provision_${name} INTERFACE)
  add_library(social_provision::${name} ALIAS social_provision_${name})
  target_include_directories(social_provision_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(social_provision_${name} INTERFACE cxx_std_20)
  target_link_libraries(social_provision_${name} INTERFACE ${ARG_LIBRARIES})
  if(SOCIAL_PROVISION_CHECKS)
    calx_add_check(check_social_provision_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES social_provision::${name} calx::random)
  endif()
endfunction()

social_provision_module(households LIBRARIES calx::linalg)
social_provision_module(care LIBRARIES nomos::games)
