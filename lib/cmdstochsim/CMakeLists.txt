cmake_minimum_required (VERSION 3.14)
project (CMDSTOCHSIM VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

add_executable(cmdstochsim "cmdstochsim.cpp")

target_include_directories(cmdstochsim
  PRIVATE
  ../../include/cmdlparser
  ../../include/expression
  ../../include/stochsim
  )

target_link_libraries (cmdstochsim PUBLIC cmdlparser)
target_link_libraries (cmdstochsim PUBLIC expression)
target_link_libraries (cmdstochsim PUBLIC stochsim)

target_compile_features(cmdstochsim PRIVATE cxx_std_17)
