#include <gtest/gtest.h>

#include <triangulation.h>

#include <cmath>
#include <cstdlib>

namespace decomposition_tests {

namespace {

double DoubleRand(double min, double max) {
    double rand_n = static_cast<double>(std::rand()) / RAND_MAX;
    return min + rand_n * (max - min);
}

}  // namespace

TEST(Performance, SimplePolygonTriangulation) {
  const size_t test_size = 10000;
  double step = M_PI_2 / test_size;
  std::vector<geom::Point2D> polygon;
  polygon.reserve(test_size);
  for (double i = 0; i < test_size; i++) {
    double angle = step * i;
    double distance = 1000;
    polygon.push_back(
        geom::Point2D(distance * std::cos(angle), distance * std::sin(angle)));
  }
  geom::Triangulate(polygon);
}

}  // decomposition_tests
