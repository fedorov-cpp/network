include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.12)

add_library(udp SHARED
        UdpClient.cpp
        UdpServer.cpp)
target_include_directories(udp PRIVATE ${PROJECT_SOURCE_DIR})
add_dependencies(udp socket)
target_link_libraries(udp socket)