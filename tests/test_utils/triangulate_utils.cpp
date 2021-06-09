#include <test_utils/triangulate_utils.h>

namespace decomposition_tests {

void TriangulationTest::TearDown() {
  size_t original_size = GetParam().size();
  if (original_size < 4)
    return;
  size_t res_sum_size = 0;
  for (const geom::Polygon2D& res_polygon : answer_) {
    EXPECT_EQ(res_polygon.Size(), 3);
    res_sum_size += res_polygon.Size();
  }
  EXPECT_EQ(original_size, res_sum_size - 2 * (answer_.size() - 1));
}

}  // decomposition_tests
