cmake_minimum_required(VERSION 3.21)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(compiler main.cpp symbol.h syntaxAnalysis.h tokens.h lexicalAnalysis.h global.h)
add_executable(test test.cpp)