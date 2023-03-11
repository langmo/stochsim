cmake_minimum_required (VERSION 3.14)
project (LEMON VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)
add_executable(lemon lemon.c)

target_compile_features(lemon PRIVATE cxx_std_17)