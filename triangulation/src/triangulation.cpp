#include <triangulation.h>

#include <decompose_to_monotones.h>
#include <polygon2d.h>
#include <triangulate_monotone.h>

#include <cassert>
#include <optional>

namespace geom {

namespace {

std::optional<Triangle2D> AsTriangle(const Polygon2D& polygon) {
  if (polygon.Size() != 3) {
    assert(false);
    return {};
  }
  Triangle2D res;
  res.a = polygon.GetAnyPoint().value();
  res.b = polygon.Next(res.a).value();
  res.c = polygon.Next(res.b).value();
  return res;
}

}  // namespace

std::vector<Triangle2D> Triangulate(const std::vector<Point2D>& polygon_v) {
  std::list<Polygon2D> y_monotones = DecomposeToYMonotones(polygon_v);
  std::vector<Triangle2D> triangles;
  for (const Polygon2D& y_monotone : y_monotones)
    for (const Polygon2D& triangle_polygon : TriangulateYMonotone(y_monotone)) {
      std::optional<Triangle2D> triangle = AsTriangle(triangle_polygon);
      if (triangle)
        triangles.push_back(triangle.value());
    }
  return triangles;
}

}  //geom
