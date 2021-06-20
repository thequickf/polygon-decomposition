#include <test_utils/decomposition_utils.h>

#include <algorithm>
#include <cstdlib>
#include <vector>

namespace decomposition_tests {

double DoubleRand(double min, double max) {
    double rand_n = static_cast<double>(std::rand()) / RAND_MAX;
    return min + rand_n * (max - min);
}

bool PolygonEqual(const geom::Polygon2D& lhp,
                  const geom::Polygon2D& rhp) {
  if (lhp.Size() != rhp.Size())
    return false;
  const geom::Polygon2D::Vertex* lh_vertex = lhp.GetAnyVertex();
  const geom::Polygon2D::Vertex* rh_vertex = lhp.GetAnyVertex();
  geom::Segment2D lh_edge = {lh_vertex->point, lh_vertex->next->point};
  geom::Segment2D rh_edge = {rh_vertex->point, rh_vertex->next->point};
  for (size_t i = 0; i < lhp.Size(); i++, lh_vertex = lh_vertex->next) {
    lh_edge = {lh_vertex->point, lh_vertex->next->point};
    if (geom::DoubleEqual(lh_edge, rh_edge))
      break;
  }
  for (size_t i = 0; i < lhp.Size();
      i++, lh_vertex = lh_vertex->next, rh_vertex = rh_vertex->next) {
    lh_edge = {lh_vertex->point, lh_vertex->next->point};
    rh_edge = {rh_vertex->point, rh_vertex->next->point};
    if (!geom::DoubleEqual(lh_edge, rh_edge))
      return false;
  }
  return true;
}

bool PolygonVectorEqual(const std::vector<geom::Point2D>& lhpv,
                        const std::vector<geom::Point2D>& rhpv) {
  if (lhpv.size() != rhpv.size())
    return false;
  if (lhpv.size() == 0 && rhpv.size() == 0)
    return true;
  auto j_it = rhpv.begin();
  for (; j_it != rhpv.end(); j_it++)
    if (geom::DoubleEqual(*j_it, lhpv.front()))
      break;
  if (j_it == rhpv.end())
    return false;
  
  for (size_t i = 0, j = j_it - rhpv.begin();
       i < lhpv.size();
       i++, j = (j + 1) % rhpv.size()) {
    if (!geom::DoubleEqual(lhpv[i], rhpv[j]))
      return false;
  }
  return true;
}

void SimpleDecompositionTest::TearDown() {
  EXPECT_EQ(answer_.size(), GetExpectedResultSize());
  for (const std::vector<geom::Point2D>& polygon_i : GetExpectedResult()) {
    bool found = false;
    for (const std::vector<geom::Point2D>& polygon_j : answer_)
      if (PolygonVectorEqual(polygon_i, polygon_j))
        found = true;
    EXPECT_TRUE(found);
  }
}

INSTANTIATE_TEST_SUITE_P(GeomUtils,
                         SimpleDecompositionTest,
                         testing::ValuesIn(simple_decomposition_cases));

void SimpleIntersectionTest::TearDown() {
  EXPECT_EQ(answer_.size(), GetExpectedResultSize());
  for (const std::vector<geom::Point2D>& polygon_i : GetExpectedResult()) {
    bool found = false;
    for (const std::vector<geom::Point2D>& polygon_j : answer_)
      if (PolygonVectorEqual(polygon_i, polygon_j))
        found = true;
    EXPECT_TRUE(found);
  }
}

INSTANTIATE_TEST_SUITE_P(GeomUtils,
                         SimpleIntersectionTest,
                         testing::ValuesIn(simple_intersection_cases));

}  // decomposition_tests
