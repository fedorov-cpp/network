include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.21)
project(network)

set(CMAKE_CXX_STANDARD 17)

include(cmake/compile_flags.cmake)

add_subdirectory(com)
add_subdirectory(socket)
add_subdirectory(tcp)
add_subdirectory(udp)
add_subdirectory(icmp)
add_subdirectory(tests)