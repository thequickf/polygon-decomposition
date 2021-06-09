#include <gtest/gtest.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>
#include <polygon2d.h>
#include <test_utils/decomposition_utils.h>

namespace decomposition_tests {

TEST_P(SimpleDecompositionTest, DecompositionWithDcel) {
  geom::Polygon2D polygon(GetInitialPolygonVector());
  geom::DcelPolygon2D dcel_polygon(polygon);
  for (const geom::Segment2D& edge : GetNewEdges())
    dcel_polygon.InsertEdge(edge);
  for (const geom::Polygon2D res_polygon : dcel_polygon.GetPolygons())
    answer_.push_back(geom::AsVector(res_polygon));
}

}  // decomposition_tests
