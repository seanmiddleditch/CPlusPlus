cmake_minimum_required(VERSION 3.4)
project(benchmarks)

add_library(benchmark STATIC IMPORTED)
set_property(TARGET benchmark PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../thirdparty/benchmark/bin/src/Release/benchmark.lib")

add_executable(benchmarks flat_map.cpp main.cpp)
include_directories(benchmarks INTERFACE "${CMAKE_SOURCE_DIR}/../thirdparty/benchmark/include")
target_link_libraries(benchmarks benchmark Shlwapi)