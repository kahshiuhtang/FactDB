cmake_minimum_required(VERSION 3.11)
project(factdb VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")
cmake_policy(SET CMP0072 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(BULLET_PHYSICS_SOURCE_DIR ${CMAKE_SOURCE_DIR})

include_directories(include)

include(FetchContent)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

add_executable(factdb 
                src/main.cpp
                )

target_include_directories(factdb PRIVATE include)