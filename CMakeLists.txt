cmake_minimum_required(VERSION 3.20)
project(raft-kv VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)

add_library(raft_core src/node.cpp src/state_machine.cpp src/transport.cpp src/cluster.cpp)

add_executable(raft_kv_cli src/main.cpp)
target_link_libraries(raft_kv_cli raft_core)

enable_testing()
add_subdirectory(tests)
