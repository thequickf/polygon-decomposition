#include <gtest/gtest.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>
#include <polygon2d.h>
#include <test_utils/decomposition_utils.h>

#include <initializer_list>
#include <optional>

namespace decomposition_tests {

TEST_P(SimpleDecompositionTest, DecompositionWithDcel) {
  geom::Polygon2D polygon(GetInitialPolygonVector());
  geom::DcelPolygon2D dcel_polygon(polygon);
  for (const geom::Segment2D& edge : GetNewEdges())
    dcel_polygon.InsertEdge(edge);
  for (const geom::Polygon2D res_polygon : dcel_polygon.GetPolygons())
    answer_.push_back(geom::AsVector(res_polygon));
}

namespace {

geom::Point2D operator*(const geom::Point2D& point, double matrix[2][2]) {
  return {point.x * matrix[0][0] + point.y * matrix[1][0],
          point.x * matrix[0][1] + point.y * matrix[1][1]};
}

geom::Segment2D operator*(const geom::Segment2D& segment, double matrix[2][2]) {
  return {segment.a * matrix, segment.b * matrix};
}

}  // namespace

struct SegmentIntercestionCase {
  geom::Segment2D a, b;
  std::optional<geom::Point2D> result;

  SegmentIntercestionCase(const geom::Segment2D& a, const geom::Segment2D& b,
                          const std::optional<geom::Point2D>& result) :
      a(a), b(b), result(result) {}
};

SegmentIntercestionCase segment_intersection_cases[] = {
  { {{-1, -1}, {1, 1}}, {{-1, 1}, {1, -1}}, {{0, 0}} },
  { {{-1, -1}, {1, 1}}, {{-1, 0}, {1, 2}}, {} },
  { {{-1, -1}, {1, 1}}, {{-2, 2}, {-1, 1}}, {} },
  { {{-1, -1}, {1, 1}}, {{0, 0}, {2, 2}}, {{0, 0}} },
  { {{-1, -1}, {1, 1}}, {{-2, 0}, {2, 0}}, {{0, 0}} },
  { {{-1, -1}, {1, 1}}, {{1, 1}, {2, 2}}, {{1, 1}} },
  { {{-1, -1}, {1, 1}}, {{1, 1}, {-2, 2}}, {{1, 1}} },
  { {{1, 1}, {2, 2}}, {{1, 1}, {2, 3}}, {{1, 1}} },
  { {{2, -2}, {2, 2}}, {{0, 0}, {4, 2}}, {{2, 1}} }
};

class IntercestionTest :
    public testing::TestWithParam<SegmentIntercestionCase> {
  void TearDown() override {
    std::optional<geom::Point2D> answer =
        geom::IntersectionPoint(GetFirstSegment(), GetSecondSegment());
    if (answer) {
      ASSERT_TRUE(GetExpectedResult());
      EXPECT_TRUE(
          geom::DoubleEqual(answer.value(), GetExpectedResult().value()));
    } else {
      EXPECT_FALSE(GetParam().result);
    }
  }
 protected:
  geom::Segment2D GetFirstSegment() {
    return GetParam().a * transfarmation_;
  }

  geom::Segment2D GetSecondSegment() {
    return GetParam().b * transfarmation_;
  }

  std::optional<geom::Point2D> GetExpectedResult() {
    if (GetParam().result)
      return GetParam().result.value() * transfarmation_;
    return {};
  }

  double transfarmation_[2][2] = { {1, 0}, {0,1} };
};

TEST_P(IntercestionTest, IntercestionPoint) {}

TEST_P(IntercestionTest, IntercestionPointReversed) {
  transfarmation_[0][0] = -1;
  transfarmation_[1][1] = -1;
}

TEST_P(IntercestionTest, IntercestionPointX1e5) {
  transfarmation_[0][0] = 1e5;
  transfarmation_[1][1] = 1e5;
}

TEST_P(IntercestionTest, IntercestionPointRotated1) {
  transfarmation_[0][0] = 0;
  transfarmation_[0][1] = -1;
  transfarmation_[1][0] = 1;
  transfarmation_[1][1] = 0;
}

TEST_P(IntercestionTest, IntercestionPointRotated2) {
  transfarmation_[0][0] = 0;
  transfarmation_[0][1] = 1;
  transfarmation_[1][0] = -1;
  transfarmation_[1][1] = 0;
}

TEST_P(IntercestionTest, IntercestionPointCustomTransformation) {
  transfarmation_[0][0] = 1;
  transfarmation_[0][1] = 2;
  transfarmation_[1][0] = 3;
  transfarmation_[1][1] = 4;
}

INSTANTIATE_TEST_SUITE_P(GeomUtils,
                         IntercestionTest,
                         testing::ValuesIn(segment_intersection_cases));

TEST_P(SimpleIntersectionTest, DcelResolveIntercestion) {
  geom::Polygon2D polygon(GetInitialPolygonVector());
  geom::DcelPolygon2D dcel_polygon(polygon);
  dcel_polygon.ResolveIntersection(GetFirstSegment(), GetSecondSegment());
  for (const geom::Polygon2D res_polygon : dcel_polygon.GetPolygons()) {
    answer_.push_back(geom::AsVector(res_polygon));
  }
}

}  // decomposition_tests
