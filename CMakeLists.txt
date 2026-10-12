cmake_minimum_required(VERSION 3.28)

project(findev)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.18.0
)
FetchContent_MakeAvailable(googletest)

add_library(
    solution

    OrderCache.cpp
)

add_executable(
    tests

    OrderCacheTest.cpp
)

target_link_libraries(
    tests

    solution
    GTest::gtest
    GTest::gtest_main
)

include_directories(.)
