#ifndef TEST_UTILS_TRIANGULATE_UTILS_H
#define TEST_UTILS_TRIANGULATE_UTILS_H

#include <gtest/gtest.h>

#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>

#include <list>

namespace decomposition_tests {

class TriangulationTest :
    public testing::TestWithParam<std::vector<geom::Point2D> > {
 public:
  void TearDown() override;
 
 protected:
   std::list<geom::Polygon2D> answer_;
};

class MonotonesTriangulationTest : public TriangulationTest {};

}  // decomposition_tests

#endif  // TEST_UTILS_TRIANGULATE_UTILS_H
