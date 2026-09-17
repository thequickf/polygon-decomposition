set(BENCHMARK_SOURCES
    triangulation_benchmarks.cpp)
add_executable(triangulation_benchmarks ${BENCHMARK_SOURCES})
target_include_directories(triangulation_benchmarks PRIVATE ../triangulation/include)
target_link_libraries(triangulation_benchmarks benchmark::benchmark ${PROJECT_NAME})
