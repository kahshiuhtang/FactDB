cmake_minimum_required(VERSION 3.11)
project(factdb VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++20")
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
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# Library for compiler flags
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

# Create a shared library from the source files in src/
add_library(factdb_lib 
    src/internal/memtable.cpp 
    src/internal/sstable.cpp
)


target_include_directories(factdb_lib PRIVATE include)

# Main executable for the src folder
add_executable(factdb 
    src/main.cpp
)

find_package(Boost 1.86 REQUIRED COMPONENTS system filesystem thread serialization)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found")
endif()
# Link the main executable with the shared library
target_link_libraries(factdb PRIVATE factdb_lib ${Boost_LIBRARIES})

# Test executable for the tests folder, linked with GTest and the shared library
add_executable(factdb_tests
    tests/test_main.cpp 
    tests/test_skiplist.cpp
    tests/test_bloomfilter.cpp
    tests/test_logger.cpp
    tests/test_memtable.cpp
    tests/test_sstable.cpp
)
find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem thread)

# Link the test executable with GTest and the shared library
target_link_libraries(factdb_tests PRIVATE gtest gtest_main factdb_lib)
target_include_directories(factdb_tests PRIVATE include)

