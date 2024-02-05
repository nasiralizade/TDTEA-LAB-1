cmake_minimum_required(VERSION 3.27)
project(labb1_v4)

set(CMAKE_CXX_STANDARD 23)

add_executable(labb1_v4 main.cpp
        lexer.h
        Ops.h
        test.h)
