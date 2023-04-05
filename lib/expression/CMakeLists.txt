cmake_minimum_required (VERSION 3.14)
project (EXPRESSION VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

# Prevent warnings about unsafe functions like 'fopen', which--according to MSVC--should be replaced by their safe
# counterparts like 'fopen_s'. These functions are not supported by other generators, and we want to be independet
# of the compiler!
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(expression "")

# Compile grammar
configure_file(
  expression_grammar.y expression_grammar.y
  COPYONLY
)
add_custom_command(
  OUTPUT ${EXPRESSION_BINARY_DIR}/expression_grammar.c
  COMMAND $<TARGET_FILE:lemon> -T${EXPRESSION_SOURCE_DIR}/expression_grammar.template ${EXPRESSION_BINARY_DIR}/expression_grammar.y
  DEPENDS lemon ${EXPRESSION_SOURCE_DIR}/expression_grammar.template ${EXPRESSION_BINARY_DIR}/expression_grammar.y
  COMMENT "Compiling ${EXPRESSION_BINARY_DIR}/expression_grammar.y to ${EXPRESSION_BINARY_DIR}/expression_grammar.c"
)

target_include_directories(expression
  PRIVATE
  ../../include/expression
  ${EXPRESSION_SOURCE_DIR}
  ${EXPRESSION_BINARY_DIR}
  )

target_sources(expression
  PRIVATE
  expression_common.cpp
  ${EXPRESSION_BINARY_DIR}/expression_grammar.c
  ExpressionParser.cpp
  )

set_source_files_properties(expression_grammar.c PROPERTIES LANGUAGE CXX )

target_compile_features(expression PRIVATE cxx_std_17)
