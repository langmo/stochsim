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

  #set(TARGET_SUFFIX ${Matlab_MEX_EXTENSION})
else ()
endif()

#if (APPLE)
#  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,-exported_symbols_list,${${Matlab_MX_LIBRARY_PATH}}/mexFunction.map")
#endif ()

#add_library(matstochsim SHARED "matstochsim.cpp")
matlab_add_mex(NAME matstochsim SHARED SRC "matstochsim.cpp")
#target_compile_definitions(matstochsim PUBLIC MATLAB_MEX_FILE)

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

#target_link_libraries(matstochsim PUBLIC 
#    ${Matlab_MAT_LIBRARY}
#    ${Matlab_MEX_LIBRARY}
#    ${Matlab_MX_LIBRARY} 
#    ${Matlab_UT_LIBRARY} 
#  )
#elseif (APPLE)
#  target_link_libraries(matstochsim PUBLIC
#    ${MATLAB_PATH}/bin/maci64/libmat.dylib
#    ${MATLAB_PATH}/bin/maci64/libmex.dylib
#    ${MATLAB_PATH}/bin/maci64/libmx.dylib
#    ${MATLAB_PATH}/bin/maci64/libut.dylib
#  )
#endif ()

#target_link_libraries (matstochsim PUBLIC cmdlparser)
#target_link_libraries (matstochsim PUBLIC expression)
#target_link_libraries (matstochsim PUBLIC stochsim)

target_link_libraries(matstochsim ${Matlab_UT_LIBRARY})
target_link_libraries (matstochsim cmdlparser)
target_link_libraries (matstochsim expression)
target_link_libraries (matstochsim stochsim)

target_compile_features(matstochsim PRIVATE cxx_std_17)

#set_target_properties(matstochsim PROPERTIES PREFIX "")
#set_target_properties(matstochsim PROPERTIES SUFFIX ".${TARGET_SUFFIX}")
