project(ashell)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

set(SRC_LIST
    src/command.cpp
    src/history.cpp
    src/main.cpp
    src/noncanmode.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
