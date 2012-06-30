# - Internal Use file for Geant4
# This file is designed for inclusion by the UseGeant4.cmake file, but
# only in the build tree. It contains functions and macros that are only
# intended to help in building the Geant4 tests.
#
# IT SHOULD NOT BE INSTALLED!

#----------------------------------------------------------------------------
# Special internal functions for building tests.
#
# - We need some internal helper modules, these should be available in
# the buildtree
include(CMakeMacroParseArguments)

#----------------------------------------------------------------------------
# function GEANT4_LINK_LIBRARY( <name> source1 source2 ...
#                               [TYPE STATIC|SHARED] 
#                               LIBRARIES library1 library2 ... )
#
function(GEANT4_LINK_LIBRARY library)
  CMAKE_PARSE_ARGUMENTS(ARG "TYPE;LIBRARIES" "" ${ARGN})
  set(sources)

  # - Fill sources
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})  
    if(IS_ABSOLUTE ${fp}) 
      file(GLOB files ${fp})     
    else()
      file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${fp})
    endif()
    if(files) 
      set(sources ${sources} ${files})
    else()
      set(sources ${sources} ${fp})
    endif()
  endforeach()

  # - Shared library unless specified
  if(NOT ARG_TYPE)
    set(ARG_TYPE SHARED)
  endif()

  # - Make sure we can access our own headers
  include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include)

  # - Deal with Win32 DLLs that don't export via declspec
  if(WIN32 AND ARG_TYPE STREQUAL SHARED)
    # - Dummy archive library
    add_library( ${library}-arc STATIC EXCLUDE_FROM_ALL ${sources})

    # - Use genwindef to create .def file listing symbols
    add_custom_command(
      OUTPUT ${library}.def
      COMMAND ${genwindef_cmd} -o ${library}.def -l ${library} ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${library}-arc.lib
      DEPENDS ${library}-arc genwindef)

    #- Dummy cpp file needed to satisfy Visual Studio.
    file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/${library}.cpp "// empty file\n" )
    add_library( ${library} SHARED ${library}.cpp ${library}.def)
    target_link_libraries(${library} ${library}-arc ${ARG_LIBRARIES})
    set_target_properties(${library} PROPERTIES LINK_INTERFACE_LIBRARIES ${ARG_LIBRARIES} ${Geant4_LIBRARIES})
  else()
    add_library( ${library} ${ARG_TYPE} ${sources})
    target_link_libraries(${library} ${ARG_LIBRARIES} ${Geant4_LIBRARIES})
  endif()

endfunction()


#----------------------------------------------------------------------------
# function GEANT4_EXECUTABLE( <name> source1 source2 ...
#                             LIBRARIES library1 library2 ... )
#
function(GEANT4_EXECUTABLE executable)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "LIBRARIES" ${ARGN})

  set(sources)

  foreach( fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      set( sources ${sources} ${files})
    else()
      set( sources ${sources} ${fp})
    endif()
  endforeach()

  include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include ${GEANT4_INCLUDE_DIR})

  add_executable(${executable} EXCLUDE_FROM_ALL ${sources})
  target_link_libraries(${executable} ${ARG_LIBRARIES} ${Geant4_LIBRARIES} )
  set_target_properties(${executable} PROPERTIES OUTPUT_NAME ${executable})
endfunction()


#----------------------------------------------------------------------------
# function GEANT4_ADD_TEST( <name> COMMAND cmd [arg1... ] 
#                           [PRECMD cmd [arg1...]] [POSTCMD cmd [arg1...]]
#                           [OUTPUT outfile] [ERROR errfile]
#                           [ENVIRONMENT var1=val1 var2=val2 ...
#                           [DEPENDS test1 ...]
#                           [TIMEOUT seconds] 
#                           [DEBUG]
#                           [SOURCE_DIR dir] [BINARY_DIR dir]
#                           [BUILD target] [PROJECT project])
#
function(GEANT4_ADD_TEST test)
  if(NOT CMAKE_PROJECT_NAME STREQUAL Geant4)
    message(WARNING "function GEANT4_ADD_TEST is only for internal Geant4 usage")
    return()
  endif()

  CMAKE_PARSE_ARGUMENTS(ARG "DEBUG" "TIMEOUT;BUILD;OUTPUT;ERROR;SOURCE_DIR;BINARY_DIR;PROJECT" 
    "COMMAND;PRECMD;POSTCMD;ENVIRONMENT;DEPENDS" ${ARGN})

  if(NOT CMAKE_GENERATOR MATCHES Makefiles)
    set(_cfg $<CONFIGURATION>/)
  endif()
  #- Handle COMMAND argument
  list(LENGTH ARG_COMMAND _len)
  if(_len LESS 1)
    if(NOT ARG_BUILD)
      message(FATAL_ERROR "GEANT4_ADD_TEST: command is mandatory (without build)")
    endif()
  else()
    list(GET ARG_COMMAND 0 _prg)
    list(REMOVE_AT ARG_COMMAND 0)
    if(NOT IS_ABSOLUTE ${_prg})
      set(_prg ${CMAKE_CURRENT_BINARY_DIR}/${_cfg}${_prg})
    elseif(EXISTS ${_prg})
    else()
	  get_filename_component(_path ${_prg} PATH)
	  get_filename_component(_file ${_prg} NAME)
      set(_prg ${_path}/${_cfg}${_file})
    endif()
    set(_cmd ${_prg} ${ARG_COMMAND})
    string(REPLACE ";" "#" _cmd "${_cmd}")
  endif()

  set(_command ${CMAKE_COMMAND} -DCMD=${_cmd})

  #- Handle PRE and POST commands
  if(ARG_PRECMD)
    set(_pre ${ARG_PRECMD})
    string(REPLACE ";" "#" _pre "${_pre}")
    set(_command ${_command} -DPRE=${_pre})
  endif()
  if(ARG_POSTCMD)
    set(_post ${ARG_POSTCMD})
    string(REPLACE ";" "#" _post "${_post}")
    set(_command ${_command} -DPOST=${_post})
  endif()

  #- Handle OUTPUT, ERROR, DEBUG arguments
  if(ARG_OUTPUT)
    set(_command ${_command} -DOUT=${ARG_OUTPUT})
  endif()

  if(ARG_ERROR)
    set(_command ${_command} -DERR=${ARG_ERROR})
  endif()

  if(ARG_DEBUG)
    set(_command ${_command} -DDBG=ON)
  endif()

  #- Handle ENVIRONMENT argument
  if(ARG_ENVIRONMENT)
    string(REPLACE ";" "#" _env "${ARG_ENVIRONMENT}")
    string(REPLACE "=" "@" _env "${_env}")
    set(_command ${_command} -DENV=${_env})
  endif()

  #- Locate the test driver
  set(_driver ${CMAKE_SOURCE_DIR}/cmake/Modules/Geant4TestDriver.cmake)
  if(NOT EXISTS ${_driver})
    message(FATAL_ERROR "GEANT4_ADD_TEST: Geant4TestDriver.cmake not found!")
  endif()
  set(_command ${_command} -P ${_driver})


  #- Now we can actually add the test
  if(ARG_BUILD)
    if(NOT ARG_SOURCE_DIR)
      set(ARG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    if(NOT ARG_BINARY_DIR)
      set(ARG_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if(NOT ARG_PROJECT)
       if(NOT PROJECT_NAME STREQUAL "Geant4")
         set(ARG_PROJECT ${PROJECT_NAME})
       else()
         set(ARG_PROJECT ${ARG_BUILD})
       endif()
    endif() 
    add_test(NAME ${test} COMMAND ${CMAKE_CTEST_COMMAND}
      --build-and-test  ${ARG_SOURCE_DIR} ${ARG_BINARY_DIR}
      --build-generator ${CMAKE_GENERATOR}
      --build-makeprogram ${CMAKE_MAKE_PROGRAM}
      --build-target ${ARG_BUILD}
      --build-project ${ARG_PROJECT}
      --build-config $<CONFIGURATION>
      --build-noclean
      --test-command ${_command} )
    set_property(TEST ${test} PROPERTY ENVIRONMENT Geant4_DIR=${CMAKE_BINARY_DIR})
  else()
    add_test(NAME ${test} COMMAND ${_command})
  endif()

  #- Handle TIMOUT and DEPENDS arguments
  if(ARG_TIMEOUT)
    set_property(TEST ${test} PROPERTY TIMEOUT ${ARG_TIMEOUT})
  endif()

  if(ARG_DEPENDS)
    set_property(TEST ${test} PROPERTY DEPENDS ${ARG_DEPENDS})
  endif()

endfunction()

