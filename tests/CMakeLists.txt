set(TEST_SOURCES
    make_monotone_tests.cpp
    performance_tests.cpp
    resolve_intersections_tests.cpp
    test_utils/decomposition_utils.cpp
    test_utils/triangulate_utils.cpp
    triangulate_monotone_tests.cpp
    triangulate_tests.cpp
    utils_tests.cpp)
add_executable(decomposition_tests ${TEST_SOURCES})
target_include_directories(decomposition_tests PRIVATE .)
target_include_directories(decomposition_tests PRIVATE ../triangulation/include)
target_include_directories(decomposition_tests PRIVATE ../triangulation/src)
target_link_libraries(decomposition_tests gtest gtest_main ${PROJECT_NAME})
add_test(decomposition_tests decomposition_tests)
