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
  if(APPLE)
    # Matlab is currently not silicon compatible.
    set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64" CACHE STRING "Either arm64 or x86_64; depending on the architecture stochsim should be compiled on MacOS.")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
  endif()
  message(STATUS "Current compiler is:      ${CMAKE_CXX_COMPILER_ID}") 
  message(STATUS "Current build type is:    ${CMAKE_BUILD_TYPE}")
  message(STATUS "Current install path is:  ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "Building for:             ${CMAKE_SYSTEM_NAME}")
  message(STATUS "Building for processor:   ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

option(BUILD_MATLAB "ON, if matstochsim should be build. Requires Matlab to be installed. The generator should be one supported by Matlab, i.e. mex and other compiled libs should be available for the compiler.")

add_subdirectory(lib/lemon)
add_subdirectory(lib/expression)
add_subdirectory(lib/stochsim)
add_subdirectory(lib/cmdlparser)
add_subdirectory(lib/cmdstochsim)
if (BUILD_MATLAB)
  message(STATUS "Building matstochsim with Matlab path \"${MATLAB_PATH}\".")
  add_subdirectory (lib/matstochsim)
endif()