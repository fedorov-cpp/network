include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.12)

add_library(tcp SHARED
        TcpClient.cpp
        TcpServer.cpp)
target_include_directories(tcp PRIVATE ${PROJECT_SOURCE_DIR})
add_dependencies(tcp socket)
target_link_libraries(tcp socket)