project(ashell)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

set(SRC_LIST
    src/main.cpp
    src/noncanmode.c
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
