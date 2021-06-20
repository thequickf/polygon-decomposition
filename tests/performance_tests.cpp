#include <gtest/gtest.h>

#include <test_utils/decomposition_utils.h>
#include <triangulation.h>

#include <cmath>

namespace decomposition_tests {

const size_t simple_polygon_test_sizes[] = {
  100,
  1000,
  10000,
  100000
};

class SimplePolygonPerformanceTest : public testing::TestWithParam<size_t> {
 public:
  void SetUp() override {
    const size_t test_size = GetParam();
    polygon_v_.reserve(test_size);
    const double step = static_cast<double>(M_PI_2) / test_size;
    for (double i = 0; i < test_size; i++) {
      const double angle = step * i;
      const double distance = 1e3;
      polygon_v_.push_back({distance * std::cos(angle),
                            distance * std::sin(angle)});
    }
  }

 protected:
  std::vector<geom::Point2D> polygon_v_;
};

TEST_P(SimplePolygonPerformanceTest, Triangulation) {
  geom::Triangulate(polygon_v_);
}

INSTANTIATE_TEST_SUITE_P(Performance,
                         SimplePolygonPerformanceTest,
                         testing::ValuesIn(simple_polygon_test_sizes));

const size_t random_polygon_test_sizes[] = {
  10,
  100,
  300
};

class RandomPolygonPerformanceTest : public testing::TestWithParam<size_t> {
 public:
  void SetUp() override {
    const size_t test_size = GetParam();
    polygon_v_.reserve(test_size);
    for (size_t i = 0; i < test_size; i++) {
      const geom::Point2D point = {DoubleRand(0, 100), DoubleRand(0, 100)};
      polygon_v_.push_back(point);
    }
  }

 protected:
  std::vector<geom::Point2D> polygon_v_;
};

TEST_P(RandomPolygonPerformanceTest, Triangulation) {
  geom::Triangulate(polygon_v_);
}

INSTANTIATE_TEST_SUITE_P(Performance,
                         RandomPolygonPerformanceTest,
                         testing::ValuesIn(random_polygon_test_sizes));

}  // decomposition_tests
