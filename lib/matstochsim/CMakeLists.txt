cmake_minimum_required (VERSION 3.14)
project (MATSTOCHSIM VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

# Find Matlab
find_package(Matlab REQUIRED COMPONENTS MX_LIBRARY MAT_LIBRARY)
get_filename_component( Matlab_MX_LIBRARY_PATH ${Matlab_MX_LIBRARY} DIRECTORY )
find_library( Matlab_UT_LIBRARY NAMES libut.lib libut.so PATHS ${Matlab_MX_LIBRARY_PATH} )
if(Matlab_FOUND)
  message(STATUS "Found Matlab installation and libraries:")
  message(STATUS "- binary dir: ${Matlab_MX_LIBRARY_PATH}")
  message(STATUS "- inclde dir: ${Matlab_INCLUDE_DIRS}")
  message(STATUS "- MX:  ${Matlab_MX_LIBRARY}")
  message(STATUS "- MEX: ${Matlab_MEX_LIBRARY}")
  message(STATUS "- MAT: ${Matlab_MAT_LIBRARY}")
  message(STATUS "- UT:  ${Matlab_UT_LIBRARY}")
else ()
  message(STATUS "Could not find Matlab installation. Configure Matlab_MX_LIBRARY_PATH, Matlab_INCLUDE_DIRS, Matlab_MX_LIBRARY, Matlab_MEX_LIBRARY, Matlab_MAT_LIBRARY and Matlab_UT_LIBRARY manually!")
endif()

matlab_add_mex(NAME matstochsim SHARED SRC "matstochsim.cpp")

target_sources(matstochsim
  PRIVATE
  MatlabProgressLogger.cpp
  MatlabStateLogger.cpp
  SimulationWrapper.cpp
  )

target_include_directories(matstochsim
  PRIVATE
  ${Matlab_INCLUDE_DIRS}
  ../../include/expression
  ../../include/cmdlparser
  ../../include/stochsim
  ../../include/matstochsim
  )

target_link_libraries(matstochsim ${Matlab_UT_LIBRARY})
target_link_libraries (matstochsim cmdlparser)
target_link_libraries (matstochsim expression)
target_link_libraries (matstochsim stochsim)

target_compile_features(matstochsim PRIVATE cxx_std_17)