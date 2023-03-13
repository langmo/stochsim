cmake_minimum_required (VERSION 3.14)
project (MATSTOCHSIM VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

if (WIN32)
  if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    #set(MATLAB_DIR "C:/SW/MATLAB/R2015B" CACHE STRING "MATLAB install directory")
    set(TARGET_SUFFIX mexw64)
    link_directories(${MATLAB_PATH}/extern/lib/win64/microsoft)
	else ()
    #set(MATLAB_DIR "C:/SW/MATLAB/R2015B" CACHE STRING "MATLAB install directory")
		set(TARGET_SUFFIX mexw32)
    link_directories(${MATLAB_PATH}/extern/lib/win32/microsoft)
	endif ()
elseif (APPLE)
  #set(MATLAB_DIR "/Applications/MATLAB_R2018b.app" CACHE STRING "MATLAB install directory")
  set(TARGET_SUFFIX mexmaci64)
  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,-exported_symbols_list,${MATLAB_PATH}/extern/lib/maci64/mexFunction.map")
else ()
  set(TARGET_SUFFIX mexa64)
endif ()

add_library(matstochsim SHARED "matstochsim.cpp")

if (WIN32)
  target_compile_definitions(matstochsim PUBLIC
    MATLAB_MEX_FILE
    _CRT_SECURE_NO_WARNINGS
    "DLL_EXPORT_SYM=__declspec(dllexport)"
  )
else ()
  target_compile_definitions(matstochsim PUBLIC MATLAB_MEX_FILE)
endif ()

target_sources(matstochsim
  PRIVATE
  MatlabProgressLogger.cpp
  MatlabStateLogger.cpp
  SimulationWrapper.cpp
  )

target_include_directories(matstochsim
  PRIVATE
  ${MATLAB_PATH}/extern/include
  ../../include/expression
  ../../include/cmdlparser
  ../../include/stochsim
  ../../include/matstochsim
  )

if (WIN32)
  find_package(Matlab REQUIRED COMPONENTS MX_LIBRARY ENG_LIBRARY MEX_COMPILER )
  target_link_libraries(matstochsim ${Matlab_MX_LIBRARY} ${Matlab_MEX_LIBRARY})
elseif (APPLE)
  target_link_libraries(matstochsim PUBLIC
    ${MATLAB_PATH}/bin/maci64/libmat.dylib
    ${MATLAB_PATH}/bin/maci64/libmex.dylib
    ${MATLAB_PATH}/bin/maci64/libmx.dylib
    ${MATLAB_PATH}/bin/maci64/libut.dylib
  )
endif ()

target_link_libraries (matstochsim PUBLIC cmdlparser)
target_link_libraries (matstochsim PUBLIC expression)
target_link_libraries (matstochsim PUBLIC stochsim)

target_compile_features(matstochsim PRIVATE cxx_std_17)

set_target_properties(matstochsim PROPERTIES PREFIX "")
set_target_properties(matstochsim PROPERTIES SUFFIX ".${TARGET_SUFFIX}")
