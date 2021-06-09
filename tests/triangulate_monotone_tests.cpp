#include <gtest/gtest.h>

#include <impl/polygon2d.hpp>
#include <impl/triangulate_monotone.hpp>
#include <impl/triangulate.hpp>
#include <test_utils/triangulate_utils.h>

namespace decomposition_tests {

const std::vector<geom::Point2D> y_monotone_polygons[] = {
  { {0, 0}, {-1, 1}, {3, 3}, {-2, 5}, {4, 6}, {5, 3}, {2, 2} },
  { {0, 1}, {1, 4}, {3, 3}, {7, 3}, {8, 0}, {6, 1} },
  { {1, 9}, {1.5, 7}, {-1, 5}, {0.2, 3.5}, {1, 2.8}, {2, 2.5}, {4, 2.4},
    {3, 2}, {2.5, 1}, {0, 0}, {-3, 1.5}, {-1, 6}, {-2, 8}, {0, 10} },
  { {-1, 9}, {-1.5, 7}, {1, 5}, {-0.2, 3.5}, {-1, 2.8}, {-2, 2.5}, {-4, 2.4},
    {-3, 2}, {-2.5, 1}, {0, 0}, {3, 1.5}, {1, 6}, {2, 8}, {0, 10} },
  { {-1, 6}, {-1, 5}, {0.2, 3.5}, {1, 2.8}, {2, 2.5}, {3, 2}, {-3, 1.5} }
};

TEST_P(MonotonesTriangulationTest, TriangulateMonotone) {
  geom::Polygon2D polygon(GetParam());
  answer_ = geom::TriangulateYMonotone(polygon);
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         MonotonesTriangulationTest,
                         testing::ValuesIn(y_monotone_polygons));

}  // decomposition_tests
