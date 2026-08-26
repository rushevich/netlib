cmake_minimum_required(VERSION 3.17)
project(cppnet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cppnet_warnings INTERFACE)
target_compile_options(cppnet_warnings INTERFACE -Wall -Wextra -Werror)

file(GLOB_RECURSE NETLIB_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(netlib STATIC ${NETLIB_SOURCES})
target_include_directories(netlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(netlib PRIVATE cppnet_warnings)

add_executable(cppnet src/main.cpp)
target_link_libraries(cppnet PRIVATE netlib cppnet_warnings)
