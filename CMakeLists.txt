cmake_minimum_required (VERSION 3.14)
project (STOCHSIMALL VERSION 0.1.0)

# Default settings if this is the main project
if (CMAKE_PROJECT_NAME STREQUAL STOCHSIMALL)
  # Make option visible in ccmake, cmake-gui
  option (BUILD_SHARED_LIBS "Build shared library" OFF)

  # Default to release build with debug info
  if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
  endif (NOT CMAKE_BUILD_TYPE)

  # Default to installing in build directory
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROFIPP_BINARY_DIR}/install
      CACHE PATH "Default install path" FORCE)
  endif()

  set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64" CACHE STRING "Either arm64 or x86_64; depending on the architecture stochsim should be compiled on MacOS.")
  #TODO: Fix
  set(CMAKE_OSX_ARCHITECTURES "x86_64")

  message(STATUS "Current build type is: ${CMAKE_BUILD_TYPE}")
  message(STATUS "Current install path is: ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
  message(STATUS "On Macs, CMAKE_APPLE_SILICON_PROCESSOR=${CMAKE_APPLE_SILICON_PROCESSOR}")
  message(STATUS "Building for processor ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

option(BUILD_MATLAB "ON, if matstochsim should be build. In this case, cache variable MATLAB_PATH has to be set.")
set(MATLAB_PATH "C:/Program Files/MATLAB/R2015a" CACHE STRING "Full path main folder of Matlab (e.g. C:/Program Files/MATLAB/R2015a). The main folder of Matlab can be recognized by containing a directory with name \"extern\".")

  # Always use standard .o suffix
#set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
#set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

add_subdirectory(lib/lemon)
add_subdirectory(lib/expression)
add_subdirectory(lib/stochsim)
add_subdirectory(lib/cmdlparser)
add_subdirectory(lib/cmdstochsim)
if (BUILD_MATLAB)
  message(STATUS "Building matstochsim with Matlab path \"${MATLAB_PATH}\".")
  add_subdirectory (lib/matstochsim)
endif()