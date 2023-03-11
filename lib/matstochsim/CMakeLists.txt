cmake_minimum_required (VERSION 3.14)
project (MATSTOCHSIM VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

add_executable(matstochsim "notexist.cpp")

target_include_directories(matstochsim
  PRIVATE
  ../../include/cmdlparser
  ../../include/expression
  ../../include/stochsim
  )

target_link_libraries (matstochsim PUBLIC cmdlparser)
target_link_libraries (matstochsim PUBLIC expression)
target_link_libraries (matstochsim PUBLIC stochsim)

target_compile_features(matstochsim PRIVATE cxx_std_17)
