cmake_minimum_required (VERSION 3.14)
project (CMDLPARSER VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

# Prevent warnings about unsafe functions like 'fopen', which--according to MSVC--should be replaced by their safe
# counterparts like 'fopen_s'. These functions are not supported by other generators, and we want to be independet
# of the compiler!
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# GCC 6+ has a warning for an ABI change due to a bug introduced in GCC 5:
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=77728. As we are not linking to other C++ code, we can just ignore it.
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif ()

add_library(cmdlparser "")

# Compile grammar
configure_file(
  cmdl_grammar.y cmdl_grammar.y
  COPYONLY
)
add_custom_command(
  OUTPUT ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.c
  COMMAND $<TARGET_FILE:lemon> -T${CMDLPARSER_SOURCE_DIR}/cmdl_grammar.template ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.y
  DEPENDS lemon ${CMDLPARSER_SOURCE_DIR}/cmdl_grammar.template ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.y
  COMMENT "Compiling ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.y to ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.c"
)

target_include_directories(cmdlparser
  PRIVATE
  ${CMDLPARSER_SOURCE_DIR}
  ${CMDLPARSER_BINARY_DIR}
  ../../include/cmdlparser
  ../../include/expression
  ../../include/stochsim
  )

target_sources(cmdlparser
  PRIVATE
  CmdlParser.cpp
  ${CMDLPARSER_BINARY_DIR}/cmdl_grammar.c
  )

set_source_files_properties(cmdl_grammar.c PROPERTIES LANGUAGE CXX )

target_link_libraries (cmdlparser PUBLIC expression)
target_link_libraries (cmdlparser PUBLIC stochsim)

target_compile_features(cmdlparser PRIVATE cxx_std_17)
