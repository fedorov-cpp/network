include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.12)

include_directories(${PROJECT_SOURCE_DIR})

add_library(icmp SHARED
        PingPort.cpp
        PingPort.hpp)
target_include_directories(icmp PRIVATE ${PROJECT_SOURCE_DIR})
add_dependencies(icmp socket)
target_link_libraries(icmp socket)
