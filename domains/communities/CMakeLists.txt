# The communities domain's C++ (ADR 0167): communities and their orders, and, as a subdomain, the society of human and AI
# communities. Header-only; each module a target communities::<module>:
#   presets       constitutions of different internal orders as data (commons, guild, cooperative, polity, league, a first
#                 league's charter, and the household from which a person's passports are issued), each admitted by thesmos,
#                 with what readers compare between them
#   circulation   gates of access classified (a personal purchasing-power gate, whatever its unit is called), provision by need
#                 within a round, a market in tokens one unit at a time, and the mediation profile read from a chronicle
#   returns       what reaches an artefact passed upstream along the graph of use, an operative's return paid to who answers for it
#   federation    recognition in levels read from each community's chronicle, compacts read across the parties' records (open
#                 compacts finding their parties there), notaries
#   claims        a claim's route and every item's route read from the record, referral, the term, a finding's completeness, review
#   registries    listings of communities within registries kept by communities, verified against each community's own profile and
#                 a witnessed head, stale and faulty listings, delisting with reasons, search across several registries (ADR 0173)
#   organisations a treaty organisation's accounts read from its record: calls for contributions under a scale, payments, arrears,
#                 core against earmarked, staff by origin, withdrawal (ADR 0173)
#   trajectories  facts about conduct recognised with their criteria, reports of one origin counted once (tekmerion), contribution
#                 over the graph of provenance (archeion), the measures of power as declared mechanisms (nomos) (ADR 0173)
# and in the subdomain human-ai/ (the society of human and AI communities):
#   lending       a bounded contribution: an agent lent under a mandate with ceilings, term and slice, receiving capacity as a
#                 guard, the handover
#   jar           circulation under scarcity: a currency set against provision by need with duties enforced by what the
#                 community knows of capacities, with free riders; the user's hypothesis of dissolution put to a construction
#
# The domain stands on the foundations alone (module.json): thesmos (acts, mandates, constitutions, the chronicle), archeion,
# symploke, kategoria, apodeixis, morphe, calx, and since ADR 0173 tekmerion, nomos and tyche (with the foundations they stand on). Inside Noetarium they are the sibling foundations, added here when no larger
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
communities_sibling(physis physis::units)
communities_sibling(tyche tyche::probability)
communities_sibling(nomos nomos::measurement)
communities_sibling(tekmerion tekmerion::evidence)

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

communities_module("" presets LIBRARIES thesmos::chronicle)
communities_module("" circulation LIBRARIES thesmos::chronicle calx::random)
communities_module("" returns LIBRARIES thesmos::kernel morphe::arithmetic)
communities_module("" federation LIBRARIES thesmos::crossings)
communities_module("" claims LIBRARIES thesmos::chronicle)
communities_module("" registries LIBRARIES thesmos::crossings)
communities_module("" organisations LIBRARIES thesmos::chronicle nomos::measurement morphe::arithmetic)
communities_module("" trajectories LIBRARIES thesmos::crossings thesmos::statements archeion::provenance nomos::games nomos::measurement tekmerion::evidence morphe::arithmetic)
communities_module("human-ai/" capacity LIBRARIES thesmos::kernel)
communities_module("human-ai/" lending LIBRARIES thesmos::chronicle)
communities_module("human-ai/" jar LIBRARIES communities::circulation thesmos::chronicle calx::random)
