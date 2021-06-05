#ifndef TEST_UTILS_DECOMPOSITION_UTILS_HPP
#define TEST_UTILS_DECOMPOSITION_UTILS_HPP

#include <gtest/gtest.h>

#include <impl/geom_utils.hpp>

#include <algorithm>
#include <vector>

namespace decomposition_tests {

bool PolygonVectorEqual(const std::vector<geom::Point2D>& lhpv,
                        const std::vector<geom::Point2D>& rhpv) {
  if (lhpv.size() != rhpv.size())
    return false;
  if (lhpv.size() == 0 && rhpv.size() == 0)
    return true;
  auto j_it = std::find(rhpv.begin(), rhpv.end(), lhpv.front());
  if (j_it == rhpv.end())
    return false;
  
  for (size_t i = 0, j = j_it - rhpv.begin();
       i < lhpv.size();
       i++, j = (j + 1) % rhpv.size()) {
    if (lhpv[i] != rhpv[j])
      return false;
  }
  return true;
}

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


const SimpleDecompositionCase simple_decomposition_cases[] = {
  {
    {
      {0, 0.5}, {-2.1, 2.9}, {-2.8, -4.1}, {-2.2, -4.9}, {6.3, 0.9}
    },
    {
      { {0, 0.5}, {-2.2, -4.9} }
    },
    {
      { {-2.8, -4.1}, {-2.1, 2.9},{0, 0.5}, {-2.2, -4.9} },
      { {6.3, 0.9}, {-2.2, -4.9}, {0, 0.5} }
    }
  },
  {
    {
      {0, 1}, {5, 0}, {6, 1}, {8, 0}, {7, 3},
      {11, 2}, {12, 4}, {10, 6}, {9, 8}, {7, 7},
      {6, 8}, {2, 7}, {4, 5}, {3, 3}, {1, 4}
    },
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


class SimpleDecomposition :
    public testing::TestWithParam<SimpleDecompositionCase> {
 public:
  void TearDown() override {
    EXPECT_EQ(answer_.size(), GetExpectedResultSize());
    for (const std::vector<geom::Point2D>& polygon_i : GetExpectedResult()) {
      bool found = false;
      for (const std::vector<geom::Point2D>& polygon_j : answer_)
        if (PolygonVectorEqual(polygon_i, polygon_j))
          found = true;
      EXPECT_TRUE(found);
    }
  }

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

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         SimpleDecomposition,
                         testing::ValuesIn(simple_decomposition_cases));

}  // decomposition_tests

#endif  // TEST_UTILS_DECOMPOSITION_UTILS_HPP
