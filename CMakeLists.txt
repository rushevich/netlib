cmake_minimum_required(VERSION 3.17)
project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings
add_library(netlib_warnings INTERFACE)
target_compile_options(netlib_warnings INTERFACE -Wall -Wextra -Werror)

# Flags
add_library(netlib_flags INTERFACE)
target_compile_options(netlib_flags INTERFACE) #-fcontracts -fcontract-evaluation-semantic=enforce#)
# The above line enables contracts and also enforces checking and termination

file(GLOB_RECURSE NETLIB_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(netlib STATIC ${NETLIB_SOURCES})
target_include_directories(netlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(netlib PRIVATE netlib_warnings netlib_flags)

add_executable(tests src/main.cpp)
target_link_libraries(tests PRIVATE netlib netlib_warnings netlib_flags)
