# The law domain's C++: facts and their kinds, proof, the sources of law, norms applied to facts, and exhibits.
# Header-only; each module a target law::<module>:
#   facts      the kinds of fact in three registers, standings and their steps, roles and logical form, traces; a record's
#              faults; the record written as a set-valued functor on the schema (schema/facts.cat), for kategoria
#   proof      burdens and declared standards of proof, met by a probability or by an argument's standing
#   sources    Article 38's sources and a domestic ranking; whom a treaty binds; which source prevails; custom by its two
#              elements under a declared scheme, with persistent objectors
#   norms      rules, exceptions, open terms and counts-as links applied to a record of facts, as a case for apodeixis
#   exhibits   exhibits as holdings with custody (archeion), authenticated, offered into a chart of evidence (tekmerion)
#   precedent  precedential constraint by Horty's reason model: a case base's consistency, the decisions it permits and forces,
#              precedents distinguished
#   proceedings a docket as archeion's sealed journal, its steps held to declared rules of procedure (order, roles, periods
#              in days), and the standing of facts moved by them
#
# The domain stands on the foundations alone (module.json): apodeixis, tekmerion, archeion and kategoria, with morphe and calx.
# Inside Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(law LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(LAW_TOP_LEVEL ON)
else()
  set(LAW_TOP_LEVEL OFF)
endif()
option(LAW_CHECKS "Build the law domain's checks" ${LAW_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(law_foundation directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/foundations/${directory}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_ROOT}/foundations/${directory} ${CMAKE_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "the law domain needs the foundation ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
law_foundation(calx calx::verification)
law_foundation(morphe morphe::combinatorics)
law_foundation(apodeixis apodeixis::interpretation)
law_foundation(kategoria kategoria::script)
law_foundation(archeion archeion::custody)
law_foundation(tekmerion tekmerion::evidence)

if(LAW_CHECKS)
  enable_testing()
endif()

function(law_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES;CHECK_LIBRARIES" ${ARGN})
  add_library(law_${name} INTERFACE)
  add_library(law::${name} ALIAS law_${name})
  target_include_directories(law_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(law_${name} INTERFACE cxx_std_20)
  target_link_libraries(law_${name} INTERFACE ${ARG_LIBRARIES})
  if(LAW_CHECKS)
    calx_add_check(check_law_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES law::${name} ${ARG_CHECK_LIBRARIES})
  endif()
endfunction()

law_module(facts LIBRARIES apodeixis::kernel morphe::combinatorics)
law_module(proof LIBRARIES law::facts apodeixis::argumentation)
law_module(sources LIBRARIES apodeixis::kernel)
law_module(norms LIBRARIES law::facts law::sources apodeixis::interpretation)
law_module(exhibits LIBRARIES law::facts archeion::custody tekmerion::evidence CHECK_LIBRARIES archeion::fixity)
law_module(precedent LIBRARIES apodeixis::kernel CHECK_LIBRARIES calx::random)
law_module(proceedings LIBRARIES law::facts archeion::fixity archeion::retention)
if(LAW_CHECKS)
  calx_add_check(check_law_schema SOURCES facts/tests/check_schema.cpp LIBRARIES law::facts kategoria::script)
  target_compile_definitions(check_law_schema PRIVATE LAW_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/schema")
endif()
