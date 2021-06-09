#include <gtest/gtest.h>

#include <impl/dcel_polygon2d.hpp>
#include <impl/geom_utils.hpp>
#include <impl/make_monotone.hpp>
#include <impl/polygon2d.hpp>
#include <test_utils/decomposition_utils.h>

#include <list>

namespace decomposition_tests {

class MakeMonotoneDecompositionTest :
    public testing::TestWithParam<std::vector<geom::Point2D> > {
 protected:
  bool IsMonotone(const geom::Polygon2D& polygon) {
    if (polygon.Size() < 3)
      return true;
    geom::Point2D current = polygon.GetAnyPoint().value();
    for (size_t i = 0; i < polygon.Size();
         i++, current = polygon.Next(current).value()) {
      geom::Polygon2D::PointType type = polygon.GetPointType(current).value();
      if (type == geom::Polygon2D::SPLIT || type == geom::Polygon2D::MERGE)
        return false;
    }
    return true;
  }
};

TEST_P(MakeMonotoneDecompositionTest, DecomposeToYMonotones) {
  geom::Polygon2D polygon(GetParam());
  std::list<geom::Polygon2D> res_polygons =
    geom::DecomposeToYMonotones(geom::AsVector(polygon));
  size_t original_size = polygon.Size();
  size_t res_size = res_polygons.size();
  size_t res_sum_size = 0;
  for (const geom::Polygon2D& res_polygon : res_polygons) {
    EXPECT_TRUE(IsMonotone(res_polygon));
    res_sum_size += res_polygon.Size();
  }
  EXPECT_EQ(original_size, res_sum_size - 2 * (res_polygons.size() - 1));
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         MakeMonotoneDecompositionTest,
                         testing::ValuesIn(test_polygons));

}  // decomposition_tests
