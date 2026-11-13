# The musicology domain's C++: pitch and tuning, exactly.
# Header-only; each module a target musicology::<module>:
#   pitch           intervals as exact frequency ratios and as primes to rational exponents, cents, octave reduction, composition
#                   and inversion, the named commas, frequencies from a reference, the beats of a mistuned consonance
#   tuning          tuning systems as scales within the octave: Pythagorean chains, sanfen sunyi, just intonation in the 5-limit,
#                   meantone and equal temperaments, temperaments that circulate, al-Urmawī's seventeen steps; their fifths and
#                   thirds against the just ones, the circle of fifths, comparison, equal divisions as homomorphisms, frequencies
#
# The domain stands on the foundations alone (module.json): morphe (arithmetic: the integers and the rationals, exactly) and
# calx (verification). Inside Noetarium they are the sibling foundations, added here when no larger project has added them.

cmake_minimum_required(VERSION 3.20)
project(musicology LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(MUSICOLOGY_TOP_LEVEL ON)
else()
  set(MUSICOLOGY_TOP_LEVEL OFF)
endif()
option(MUSICOLOGY_CHECKS "Build the musicology domain's checks" ${MUSICOLOGY_TOP_LEVEL})

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
    message(FATAL_ERROR "the musicology domain needs the foundations: build it inside Noetarium")
  endif()
endif()
if(NOT TARGET morphe::arithmetic)
  add_subdirectory(${SCIENCE_ROOT}/foundations/morphe ${CMAKE_CURRENT_BINARY_DIR}/morphe)
endif()

if(MUSICOLOGY_CHECKS)
  enable_testing()
endif()

function(musicology_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
  add_library(musicology_${name} INTERFACE)
  add_library(musicology::${name} ALIAS musicology_${name})
  target_include_directories(musicology_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(musicology_${name} INTERFACE cxx_std_20)
  target_link_libraries(musicology_${name} INTERFACE ${ARG_LIBRARIES})
  if(MUSICOLOGY_CHECKS)
    calx_add_check(check_musicology_${name} SOURCES ${name}/tests/check_${name}.cpp LIBRARIES musicology::${name})
  endif()
endfunction()

musicology_module(pitch LIBRARIES morphe::arithmetic)
musicology_module(tuning LIBRARIES musicology::pitch)
