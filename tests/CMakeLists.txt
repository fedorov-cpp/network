# GTEST
set(GTEST_INCLUDE_DIR /usr/include/gtest)
set(GTEST_LIB_DIR /usr/lib64)

# TEST
enable_testing()

# tests
enable_testing()
set(TESTS
        test_icmp.cpp
        test_com.cpp
        test_interface.cpp
        test_udp.cpp
        test_tcp.cpp)
add_executable(tests ${TESTS})
target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})
add_dependencies(tests icmp com tcp udp)
target_link_libraries(tests gtest gtest_main icmp com tcp udp)
add_test(NAME tests COMMAND tests)