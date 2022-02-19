set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

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