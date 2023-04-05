cmake_minimum_required (VERSION 3.14)
project (LEMON VERSION 0.1.0)
cmake_policy(SET CMP0115 NEW)

# Prevent warnings about unsafe functions like 'fopen', which--according to MSVC--should be replaced by their safe
# counterparts like 'fopen_s'. These functions are not supported by other generators, and we want to be independet
# of the compiler!
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(lemon lemon.c)

target_compile_features(lemon PRIVATE cxx_std_17)