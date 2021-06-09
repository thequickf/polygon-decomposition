#ifndef TEST_UTILS_DECOMPOSITION_UTILS_H
#define TEST_UTILS_DECOMPOSITION_UTILS_H

#include <gtest/gtest.h>

#include <impl/geom_utils.hpp>

#include <vector>

namespace decomposition_tests {

bool PolygonVectorEqual(const std::vector<geom::Point2D>& lhpv,
                        const std::vector<geom::Point2D>& rhpv);

struct SimpleDecompositionCase {
  const std::vector<geom::Point2D> polygon_v;
  const std::vector<geom::Segment2D> new_edges;
  const std::vector<std::vector<geom::Point2D> > result;

  SimpleDecompositionCase(
      const std::vector<geom::Point2D>& polygon_v,
      const std::vector<geom::Segment2D>& new_edges,
      const std::vector<std::vector<geom::Point2D> >& result) :
      polygon_v(polygon_v), new_edges(new_edges), result(result) {}
};


const std::vector<geom::Point2D> test_polygons[] = {
  {
    {0, 0.5}, {-2.1, 2.9}, {-2.8, -4.1}, {-2.2, -4.9}, {6.3, 0.9}
  },
  {
    {0, 1}, {5, 0}, {6, 1}, {8, 0}, {7, 3},
    {11, 2}, {12, 4}, {10, 6}, {9, 8}, {7, 7},
    {6, 8}, {2, 7}, {4, 5}, {3, 3}, {1, 4}
  }
};

const SimpleDecompositionCase simple_decomposition_cases[] = {
  {
    test_polygons[0],
    {
      { {0, 0.5}, {-2.2, -4.9} }
    },
    {
      { {-2.8, -4.1}, {-2.1, 2.9},{0, 0.5}, {-2.2, -4.9} },
      { {6.3, 0.9}, {-2.2, -4.9}, {0, 0.5} }
    }
  },
  {
    test_polygons[1],
    {
      { {5, 0}, {3, 3} },
      { {6, 1}, {3, 3} },
      { {6, 1}, {7, 3} },
      { {7, 3}, {7, 7} },
      { {0, 1}, {3, 3} },
      { {7, 7}, {10, 6} },
      { {4, 5}, {7, 3} }
    },
    {
      { {0, 1}, {1, 4}, {3, 3} },
      { {0, 1}, {3, 3}, {5, 0} },
      { {3, 3}, {6, 1}, {5, 0} },
      { {3, 3}, {4, 5}, {7, 3}, {6, 1} },
      { {6, 1}, {7, 3}, {8, 0} },
      { {2, 7}, {6, 8}, {7, 7}, {7, 3}, {4, 5} },
      { {7, 3}, {7, 7}, {10, 6}, {12, 4}, {11, 2} },
      { {7, 7}, {9, 8}, {10, 6} }
    }
  }
};

class SimpleDecompositionTest :
    public testing::TestWithParam<SimpleDecompositionCase> {
 public:
  void TearDown() override;

 protected:
  std::vector<geom::Point2D> GetInitialPolygonVector() const {
    return GetParam().polygon_v;
  }

  std::vector<geom::Segment2D> GetNewEdges() const {
    return GetParam().new_edges;
  }

  std::vector<std::vector<geom::Point2D> > GetExpectedResult() const {
    return GetParam().result;
  }

  size_t GetExpectedResultSize() const {
    return GetParam().result.size();
  }

  std::vector<std::vector<geom::Point2D> > answer_;
};

}  // decomposition_tests

#endif  // TEST_UTILS_DECOMPOSITION_UTILS_H
