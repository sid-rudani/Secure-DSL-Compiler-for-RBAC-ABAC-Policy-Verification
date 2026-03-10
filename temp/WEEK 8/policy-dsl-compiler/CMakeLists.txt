cmake_minimum_required(VERSION 3.10)
project(policy-dsl-compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
# Source files
# set(SOURCES
#     src/lexer/Lexer.cpp
#     src/lexer/Token.cpp
#     src/lexer/TokenType.cpp
#     src/parser/Parser.cpp
#     src/parser/AST.cpp
#     src/semantic/SymbolTable.cpp
#     src/semantic/SemanticAnalyzer.cpp
#     src/semantic/TypeChecker.cpp
#     src/analyzer/ConflictDetector.cpp
#     src/analyzer/PrivilegeEscalation.cpp
#     src/analyzer/RedundancyDetector.cpp
#     src/codegen/AccessMatrix.cpp
#     src/codegen/MatrixGenerator.cpp
#     src/utils/Error.cpp
#     src/utils/JsonOutput.cpp
#     src/main.cpp
# )

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Main executable
add_executable(compiler ${SOURCES})

# Testing - disabled due to missing Google Test dependency
# enable_testing()
# add_executable(test_lexer tests/test_lexer.cpp 
#     src/lexer/Lexer.cpp
#     src/lexer/Token.cpp
#     src/lexer/TokenType.cpp
#     src/utils/Error.cpp
# )
# add_test(NAME LexerTest COMMAND test_lexer)
# 
# add_executable(test_parser tests/test_parser.cpp
#     src/lexer/Lexer.cpp
#     src/lexer/Token.cpp
#     src/lexer/TokenType.cpp
#     src/parser/Parser.cpp
#     src/parser/AST.cpp
#     src/utils/Error.cpp
# )
# add_test(NAME ParserTest COMMAND test_parser)
