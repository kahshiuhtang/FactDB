cmake_minimum_required(VERSION 3.11)
project(factdb VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")
cmake_policy(SET CMP0072 NEW)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(include)

# Fetch GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library for compiler flags
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

# Create a shared library from the source files in src/
add_library(factdb_lib 
    src/main.cpp  # List the source files you want to share between the executables
)

target_include_directories(factdb_lib PRIVATE include)

# Main executable for the src folder
add_executable(factdb 
    src/main.cpp
    src/internal/skiplist.cpp 
    src/internal/sstable.cpp
)

# Link the main executable with the shared library
target_link_libraries(factdb PRIVATE factdb_lib)

# Test executable for the tests folder, linked with GTest and the shared library
add_executable(factdb_tests
    tests/test_main.cpp  # Add your test files here
)

# Link the test executable with GTest and the shared library
target_link_libraries(factdb_tests PRIVATE gtest gtest_main factdb_lib)
target_include_directories(factdb_tests PRIVATE include)