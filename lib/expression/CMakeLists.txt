cmake_minimum_required (VERSION 3.14)
project (EXPRESSION VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

# Compile grammar
add_custom_command(
  OUTPUT ${EXPRESSION_SOURCE_DIR}/expression_grammar.c
  COMMAND ${CMAKE_BINARY_DIR}/lib/lemon/lemon -T${EXPRESSION_SOURCE_DIR}/expression_grammar.template ${EXPRESSION_SOURCE_DIR}/expression_grammar.y
  DEPENDS ${EXPRESSION_SOURCE_DIR}/expression_grammar.template ${EXPRESSION_SOURCE_DIR}/expression_grammar.y
)

add_library(expression "")

target_include_directories(expression
  PRIVATE
  ../../include/expression
  )

target_sources(expression
  PRIVATE
  expression_common.cpp
  expression_grammar.c
  ExpressionParser.cpp
  )

set_source_files_properties(expression_grammar.c PROPERTIES LANGUAGE CXX )

target_compile_features(expression PRIVATE cxx_std_17)
