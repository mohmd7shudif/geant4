#------------------------------------------------------------------------------
# Top Level CMakeLists.txt for Geant4 Build
#
# 21st September 2010 Ben Morgan
#
# $Id: CMakeLists.txt,v 1.6 2010-12-08 16:36:34 bmorgan Exp $
# GEANT4 Tag $Name: not supported by cvs2svn $
#

#------------------------------------------------------------------------------
# - Enforce an out-of-source builds before anything else
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "Geant4 requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
endif()

#------------------------------------------------------------------------------
# - Define CMake requirements and override make rules as needed
#
cmake_minimum_required(VERSION 2.6)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_SOURCE_DIR}/cmake/Modules/Geant4MakeRules_cxx.cmake)

#set(CMAKE_USER_MAKE_RULES_OVERRIDE_Fortran
#   ${CMAKE_SOURCE_DIR}/cmake/Modules/Geant4MakeRules_fortran.cmake)

#------------------------------------------------------------------------------
# - Project definition
#
project(Geant4)


#----------------------------------------------------------------------------
# - Prepend our own CMake Modules to the search path
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
set(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH})


#----------------------------------------------------------------------------
# - Add functionality provided by standard and custom modules
# See the documentation in each of these modules for further details.
#
# - Versioning. We do this here for now
set(${PROJECT_NAME}_VERSION "9.5.1")
set(${PROJECT_NAME}_VERSION_MAJOR "9")
set(${PROJECT_NAME}_VERSION_MINOR "5")
set(${PROJECT_NAME}_VERSION_PATCH "1")

# - Provide dependent options as these are needed for some Geant4 features
include(CMakeDependentOption)

# - Provide general CMake utilities for Geant4.
include(Geant4MacroUtilities)

# - Provide an 'uninstall' target.
include(CMakeUninstallTarget)

# - Provide the 'validate_sources' target.
include(Geant4ValidateSources)

# - Provide standard install directories permitting customization.
include(Geant4InstallDirs)

# - Provide options to control how Geant4 libraries are built
include(Geant4LibraryBuildOptions)

# - Provide interface to control use of optional components
include(Geant4OptionalComponents)

# - Provide interface to control use of UI/Vis components
#   Written in a separate module from other optional components because
#   there are many complex options to handle.
include(Geant4InterfaceOptions)

# - Provide options to enable wrapping of Geant4 by other languages
include(Geant4Wrapping)


#------------------------------------------------------------------------------
# Add the source and environments subdirectories
# source       : Process all the Geant4 core targets
# environments : Process optional wrappings of Geant4 (NOTYETIMPLEMENTED)
add_subdirectory(source)
#add_subdirectory(environments)


#----------------------------------------------------------------------------
# - Perform all post build tasks
# At the CMake level, this simply means that we must know about targets
# and other properties processed in source and environments trees before 
# these tasks can be performed.
#
#----------------------------------------------------------------------------
# Installation of optional read-only architecture independent data files.
# E.g. Examples, data libraries, documentation.
# Done before toolchain generation because it may affect what we have to do
# there!
#
include(Geant4InstallData)

#------------------------------------------------------------------------------
# Generate any Use/Config files here once everything else has been processed
# e.g. "UseGeant4.cmake", "Geant4Config.cmake" - library dependencies etc
#
include(Geant4ToolchainBackwardCompatibility)
include(Geant4ConfigureConfigScript)

#----------------------------------------------------------------------------
# Create the Geant4Config files and supporting modules
#
include(Geant4BuildProjectConfig)

#------------------------------------------------------------------------------
# Add the examples and tests subdirectories
# This is done after the Geant4Config.cmake file has been generated
if(GEANT4_ENABLE_TESTING)
  include(Geant4CTest)
  add_subdirectory(tests)
  if(EXISTS ${CMAKE_SOURCE_DIR}/benchmarks)
    add_subdirectory(benchmarks)
  endif()
endif()
if(GEANT4_BUILD_EXAMPLES)
  set(Geant4_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Current build directory")
  add_subdirectory(examples)
endif()
#-----------------------------------------------------------------------------
# Provide packaging with CPack.
include(Geant4CPackBase)

#----------------------------------------------------------------------------
# Final output - show what's been enabled so that user knows what's
# happening - also useful for later problem solving!
#
GEANT4_PRINT_ENABLED_FEATURES()

