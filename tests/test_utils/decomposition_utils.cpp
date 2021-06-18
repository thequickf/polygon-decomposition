#include <test_utils/decomposition_utils.h>

#include <algorithm>
#include <vector>

namespace decomposition_tests {

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
