#ifndef TEST_UTILS_DECOMPOSITION_UTILS_H
#define TEST_UTILS_DECOMPOSITION_UTILS_H

#include <gtest/gtest.h>

#include <geom_utils.h>

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

const std::vector<geom::Point2D> self_intersecting_polygons[] = {
  { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },
  { {-1, -1}, {-2, 0}, {-1, 1}, {1, -1}, {2, 0}, {1, 1} }
};

const std::vector<geom::Point2D> test_polygons[] = {
  {
    {0, 0.5}, {-2.1, 2.9}, {-2.8, -4.1}, {-2.2, -4.9}, {6.3, 0.9}
  },
  {
    {0, 1}, {5, 0}, {6, 1}, {8, 0}, {7, 3},
    {11, 2}, {12, 4}, {10, 6}, {9, 8}, {7, 7},
    {6, 8}, {2, 7}, {4, 5}, {3, 3}, {1, 4}
  },
  {
    {103, 100}, {148, 64}, {256, 35}, {388, 35}, {488, 55},
    {575, 148}, {558, 271}, {467, 368}, {338, 378}, {197, 380},
    {132, 283}, {137, 163}, {245, 102}, {373, 101}, {466, 137},
    {487, 235}, {426, 297}, {349, 345}, {232, 321}, {212, 211},
    {251, 147}, {333, 147}, {398, 174}, {380, 250}, {348, 300},
    {285, 291}, {274, 223}, {301, 177}, {333, 193}, {321, 238},
    {343, 247}, {372, 194}, {314, 162}, {266, 181}, {249, 233},
    {254, 287}, {281, 312}, {349, 321}, {419, 268}, {434, 200},
    {399, 136}, {257, 128}, {186, 172}, {171, 264}, {195, 337},
    {282, 361}, {416, 347}, {501, 287}, {526, 188}, {470, 104},
    {325, 68}, {186, 85}, {125, 141}
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

struct SimpleIntersectionCase {
  const std::vector<geom::Point2D> polygon_v;
  const geom::Segment2D first_s, second_s;
  const std::vector<std::vector<geom::Point2D> > result;

  SimpleIntersectionCase(
      const std::vector<geom::Point2D>& polygon_v,
      const geom::Segment2D& first_s, const geom::Segment2D& second_s,
      const std::vector<std::vector<geom::Point2D> >& result) :
      polygon_v(polygon_v), first_s(first_s), second_s(second_s), result(result) {}
};

const SimpleIntersectionCase simple_intersection_cases[] = {
  {
    self_intersecting_polygons[0],
    {{-1, 1}, {1, -1}}, {{-1, -1}, {1, 1}},
    {{{-1, 1}, {0, 0}, {-1, -1}}, {{1, 1}, {1, -1}, {0, 0}}}
  },
  {
    self_intersecting_polygons[1],
    {{-1, 1}, {1, -1}}, {{-1, -1}, {1, 1}},
    {{{-1, 1}, {0, 0}, {-1, -1}, {-2, 0}}, {{1, 1}, {2, 0}, {1, -1}, {0, 0}}}
  },
  {
    self_intersecting_polygons[1],
    {{1, -1}, {-1, 1}}, {{-1, -1}, {1, 1}},
    {{{-1, 1}, {0, 0}, {-1, -1}, {-2, 0}}, {{1, 1}, {2, 0}, {1, -1}, {0, 0}}}
  },
  {
    self_intersecting_polygons[1],
    {{-1, 1}, {1, -1}}, {{1, 1}, {-1, -1}},
    {{{-1, 1}, {0, 0}, {-1, -1}, {-2, 0}}, {{1, 1}, {2, 0}, {1, -1}, {0, 0}}}
  },
  {
    self_intersecting_polygons[1],
    {{1, -1}, {-1, 1}}, {{1, 1}, {-1, -1}},
    {{{-1, 1}, {0, 0}, {-1, -1}, {-2, 0}}, {{1, 1}, {2, 0}, {1, -1}, {0, 0}}}
  }
};

class SimpleIntersectionTest :
    public testing::TestWithParam<SimpleIntersectionCase> {
 public:
  void TearDown() override;

 protected:
  std::vector<geom::Point2D> GetInitialPolygonVector() const {
    return GetParam().polygon_v;
  }

  geom::Segment2D GetFirstSegment() const {
    return GetParam().first_s;
  }

  geom::Segment2D GetSecondSegment() const {
    return GetParam().second_s;
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
