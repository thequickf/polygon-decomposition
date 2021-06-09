#ifndef IMPL_TRIANGULATE_HPP
#define IMPL_TRIANGULATE_HPP

#include <impl/geom_utils.hpp>
#include <impl/make_monotone.hpp>
#include <impl/triangulate_monotone.hpp>

#include <list>
#include <vector>

namespace geom {

inline std::list<Polygon2D> Triangulate(const std::vector<Point2D>& polygon_v) {
  std::list<Polygon2D> y_monotones = DecomposeToYMonotones(polygon_v);
  std::list<Polygon2D> triangles;
  for (const Polygon2D& y_monotone : y_monotones)
    for (const Polygon2D& triangle : TriangulateYMonotone(y_monotone))
      triangles.push_back(triangle);
  return triangles;
}

}  // geom

#endif  // IMPL_TRIANGULATE_HPP
