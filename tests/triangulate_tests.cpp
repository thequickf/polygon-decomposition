#include <gtest/gtest.h>

#include <impl/triangulate.hpp>
#include <test_utils/decomposition_utils.h>
#include <test_utils/triangulate_utils.h>

namespace decomposition_tests {

TEST_P(TriangulationTest, Triangulate) {
  answer_ = geom::Triangulate(GetParam());
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         TriangulationTest,
                         testing::ValuesIn(test_polygons));

}  // decomposition_tests
