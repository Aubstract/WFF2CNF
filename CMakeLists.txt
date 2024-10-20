cmake_minimum_required(VERSION 3.28)
project(WFF2CNF)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(WFF2CNF
        src/main.cpp
        src/AST.hpp
        src/AST.cpp
        src/Token.hpp
        src/Operators.hpp
        src/Transformer.hpp
        src/Transformer.cpp
        src/Operators.cpp
)
