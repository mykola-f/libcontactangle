cmake_minimum_required(VERSION 3.1)
project(contactangletest)

enable_testing()
find_package(GTest MODULE REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

# set include source directory
include_directories(../src)

add_executable(tests
    geometry_ut.cpp
)

target_link_libraries(tests
                      GTest::GTest
                      GTest::Main
                      contactangle
)

add_test(
    NAME tests
    COMMAND tests/tests
)