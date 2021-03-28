include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.12)

add_library(com SHARED
        ComPort.cpp
        ComPort.hpp)
target_include_directories(com PRIVATE ${PROJECT_SOURCE_DIR})