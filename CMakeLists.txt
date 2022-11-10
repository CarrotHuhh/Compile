cmake_minimum_required(VERSION 3.21)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compiler
        global.h
        lexicalAnalysis.h
        main.cpp
        symbol.h
        syntaxAnalysis.h
        tokens.h)
