cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(name load_gdml)
project(${name})
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Geant4_INCLUDE_DIR})
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

add_executable(${name} EXCLUDE_FROM_ALL ${name}.cc ${sources})
target_link_libraries(${name} ${Geant4_LIBRARIES})
