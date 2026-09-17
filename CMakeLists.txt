cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
project(triangulation)

option(BUILD_TESTS "Build triangulation tests." ON)
option(BUILD_BENCHMARKS "Build triangulation benchmarks." OFF)

ADD_SUBDIRECTORY(triangulation)

if(BUILD_TESTS)
  enable_testing()
  ADD_SUBDIRECTORY(third_party/googletest)
  ADD_SUBDIRECTORY(tests)
endif(BUILD_TESTS)

if(BUILD_BENCHMARKS)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
  ADD_SUBDIRECTORY(third_party/benchmark)
  ADD_SUBDIRECTORY(benchmarks)
endif(BUILD_BENCHMARKS)
