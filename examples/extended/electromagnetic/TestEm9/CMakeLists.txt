cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(TestEm9)
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Geant4_INCLUDE_DIR})
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

add_executable(TestEm9 EXCLUDE_FROM_ALL TestEm9.cc ${sources})
target_link_libraries(TestEm9 ${Geant4_LIBRARIES})
