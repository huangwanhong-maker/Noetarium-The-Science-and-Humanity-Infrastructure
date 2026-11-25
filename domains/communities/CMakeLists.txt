# The communities domain's C++ (ADR 0167): communities and their orders, and, as a subdomain, the society of human and AI
# communities. Header-only; each module a target communities::<module>:
#   presets       constitutions of different internal orders as data (commons, guild, cooperative, polity, league, and a first
#                 league's charter), each admitted by thesmos, with what readers compare between them
#   circulation   gates of access classified (a personal purchasing-power gate, whatever its unit is called), provision by need
#                 within a round, a market in tokens one unit at a time, and the mediation profile read from a chronicle
# and in the subdomain human-ai/ (the society of human and AI communities):
#   lending       a bounded contribution: an agent lent under a mandate with ceilings, term and slice, receiving capacity as a
#                 guard, the handover
#   jar           circulation under scarcity: a currency set against provision by need with duties enforced by what the
#                 community knows of capacities, with free riders; the user's hypothesis of dissolution put to a construction
#
# The domain stands on the foundations alone (module.json): thesmos (acts, mandates, constitutions, the chronicle), archeion,
# symploke, kategoria, apodeixis, morphe, calx. Inside Noetarium they are the sibling foundations, added here when no larger
# project has added them.

cmake_minimum_required(VERSION 3.20)
project(communities LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(COMMUNITIES_TOP_LEVEL ON)
else()
  set(COMMUNITIES_TOP_LEVEL OFF)
endif()
option(COMMUNITIES_CHECKS "Build the communities domain's checks" ${COMMUNITIES_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SCIENCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
function(communities_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${SCIENCE_ROOT}/foundations/${directory}/CMakeLists.txt)
      add_subdirectory(${SCIENCE_ROOT}/foundations/${directory} ${CMAKE_CURRENT_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "the communities domain needs the foundation ${directory}: build it inside Noetarium")
    endif()
  endif()
endfunction()
communities_sibling(calx calx::verification)
communities_sibling(morphe morphe::formal)
communities_sibling(apodeixis apodeixis::kernel)
communities_sibling(archeion archeion::fixity)
communities_sibling(symploke symploke::jsonld)
communities_sibling(kategoria kategoria::script)
communities_sibling(thesmos thesmos::chronicle)

if(COMMUNITIES_CHECKS)
  enable_testing()
endif()

# communities_module(<directory> <name> [LIBRARIES ...]): the target communities::<name> from <directory>/<name>, and its check.
function(communities_module directory name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  set(dir ${CMAKE_CURRENT_SOURCE_DIR}/${directory}${name})
  add_library(communities_${name} INTERFACE)
  add_library(communities::${name} ALIAS communities_${name})
  target_include_directories(communities_${name} INTERFACE ${dir}/include)
  target_compile_features(communities_${name} INTERFACE cxx_std_20)
  target_link_libraries(communities_${name} INTERFACE ${ARG_LIBRARIES})
  if(COMMUNITIES_CHECKS AND EXISTS ${dir}/tests/check_${name}.cpp)
    calx_add_check(check_communities_${name} SOURCES ${dir}/tests/check_${name}.cpp LIBRARIES communities::${name} calx::random)
    target_compile_definitions(check_communities_${name} PRIVATE COMMUNITIES_DATA="${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endfunction()

communities_module("" presets LIBRARIES thesmos::constitutions)
communities_module("" circulation LIBRARIES thesmos::chronicle calx::random)
communities_module("" returns LIBRARIES thesmos::kernel morphe::arithmetic)
communities_module("" federation LIBRARIES thesmos::crossings)
communities_module("" claims LIBRARIES thesmos::chronicle)
communities_module("human-ai/" capacity LIBRARIES thesmos::kernel)
communities_module("human-ai/" lending LIBRARIES thesmos::chronicle)
communities_module("human-ai/" jar LIBRARIES communities::circulation thesmos::chronicle calx::random)
