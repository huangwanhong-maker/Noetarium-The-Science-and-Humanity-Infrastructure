# The policy domain's C++: evidence-based policy making (ADR 0114). Header-only; each module a target policy::<module>:
#   change      theories of change: an acyclic graph of causal claims with their assumptions, support factors and evidence; the
#               strongest path and its weakest link, gaps on every path, Wright's rule with an enclosure
#   evidence    the evidence for a claim: studies pooled by peira and graded by a declared scheme (tekmerion's warrant), factors of
#               the synthesis read off it by declared rules
#   transfer    from there to here: Cartwright and Hardie's argument with the support factors a path needs, and the effect to
#               expect, carried over or transported through strata by peira
#   options     options appraised: a net present value normal in the effect (nomos's appraisal), the switching worth of a unit, the
#               value of knowing which option is best (tyche's decision table over draws)
#   adoption    adopt, reject or study first: EVPI and EVSI for a normal net value (tyche), the study worth doing, the robustness
#               of the choice over bounds on a unit's worth
#
# The domain stands on the foundations alone (module.json): tekmerion (warrant), peira (synthesis), nomos (appraisal), tyche
# (decision), apodeixis (verdicts), morphe (acyclic relations), calx (random streams, verification), and physis as the siblings'
# order requires. Inside Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(policy LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(POLICY_TOP_LEVEL ON)
else()
  set(POLICY_TOP_LEVEL OFF)
endif()
option(POLICY_CHECKS "Build the policy domain's checks" ${POLICY_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(policy_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/foundations/${directory}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_ROOT}/foundations/${directory} ${CMAKE_CURRENT_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "the policy domain needs the foundation ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
policy_sibling(calx calx::verification)
policy_sibling(morphe morphe::combinatorics)
policy_sibling(physis physis::units)
policy_sibling(tyche tyche::decision)
policy_sibling(peira peira::synthesis)
policy_sibling(nomos nomos::appraisal)
policy_sibling(apodeixis apodeixis::kernel)
policy_sibling(tekmerion tekmerion::warrant)

if(POLICY_CHECKS)
  enable_testing()
endif()

function(policy_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(policy_${name} INTERFACE)
  add_library(policy::${name} ALIAS policy_${name})
  target_include_directories(policy_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(policy_${name} INTERFACE cxx_std_20)
  target_link_libraries(policy_${name} INTERFACE ${ARG_LIBRARIES})
  if(POLICY_CHECKS)
    calx_add_check(check_policy_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES policy::${name} calx::random)
  endif()
endfunction()

policy_module(change LIBRARIES apodeixis::kernel morphe::combinatorics)
policy_module(evidence LIBRARIES policy::change peira::synthesis tekmerion::warrant)
policy_module(transfer LIBRARIES policy::change peira::synthesis tekmerion::warrant)
policy_module(options LIBRARIES apodeixis::kernel nomos::appraisal tyche::decision calx::random)
policy_module(adoption LIBRARIES policy::options tyche::decision)
policy_module(learning LIBRARIES policy::change peira::survival)
