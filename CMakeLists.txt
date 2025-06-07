cmake_minimum_required(VERSION 3.31)
project(untitled2)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled2 main.cpp game.cpp info.cpp
        gameword.h
        gameword.cpp
        info.h
        game.h
)
