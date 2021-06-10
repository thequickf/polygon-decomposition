cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
project(triangulation)

option(BUILD_TESTS "Build triangulation tests." ON)

ADD_SUBDIRECTORY(triangulation)

if(BUILD_TESTS)
  enable_testing()
  ADD_SUBDIRECTORY(third_party/googletest)
  ADD_SUBDIRECTORY(tests)
endif(BUILD_TESTS)
