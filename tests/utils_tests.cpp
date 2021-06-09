#include <gtest/gtest.h>

#include <impl/dcel_polygon2d.hpp>
#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>
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
