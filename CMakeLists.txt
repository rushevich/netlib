cmake_minimum_required(VERSION 3.15)
project(cppnet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cppnet_warnings INTERFACE)
target_compile_options(cppnet_warnings INTERFACE -Wall -Wextra -Werror)
add_library(netlib STATIC socket.cpp)
target_include_directories(netlib PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(netlib PRIVATE cppnet_warnings)

add_executable(cppnet main.cpp)
target_link_libraries(cppnet PRIVATE netlib cppnet_warnings)
