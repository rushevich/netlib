cmake_minimum_required(VERSION 3.20)
project(netlib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings
add_library(netlib_warnings INTERFACE)
target_compile_options(netlib_warnings INTERFACE -Wall -Wextra -Werror -Wpedantic)

# Flags
add_library(netlib_flags INTERFACE)
target_compile_options(netlib_flags INTERFACE) #-fcontracts -fcontract-evaluation-semantic=enforce#)
# The above line enables contracts and also enforces checking and termination

# Implementations discovery
file(GLOB_RECURSE NETLIB_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Creating the static library
add_library(netlib STATIC ${NETLIB_SOURCES})
target_include_directories(netlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(netlib PRIVATE netlib_warnings netlib_flags)

# Testing setup & googletest setup
# https://cmake.org/cmake/help/latest/module/FindGTest.html
enable_testing()
find_package(GTest REQUIRED)
add_executable(netlib_tests tests/tests.cpp)
target_link_libraries(netlib_tests PRIVATE netlib netlib_warnings netlib_flags GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(netlib_tests)
