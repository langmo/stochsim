cmake_minimum_required (VERSION 3.14)
project (STOCHSIM VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

add_library(stochsim "Simulation.cpp")

target_include_directories(stochsim
  PRIVATE
  ../../include/expression
  ../../include/stochsim
  )

target_link_libraries (stochsim PUBLIC expression)

target_compile_features(stochsim PRIVATE cxx_std_17)
