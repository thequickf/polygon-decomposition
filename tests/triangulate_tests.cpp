#include <gtest/gtest.h>

#include <triangulation_base_geometry.h>
#include <triangulation.h>
#include <test_utils/decomposition_utils.h>
#include <test_utils/triangulate_utils.h>

#include <vector>

namespace decomposition_tests {

TEST_P(TriangulationTest, Triangulate) {
  std::vector<geom::Triangle2D> triangles = geom::Triangulate(GetParam());
  for (const geom::Triangle2D triangle : triangles) {
    answer_.push_back(geom::Polygon2D({triangle.a, triangle.b, triangle.c}));
  }
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         TriangulationTest,
                         testing::ValuesIn(test_polygons));

}  // decomposition_tests
