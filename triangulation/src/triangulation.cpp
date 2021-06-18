#include <triangulation.h>

#include <decompose_to_monotones.h>
#include <polygon2d.h>
#include <resolve_intersections.h>
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
  const Polygon2D::Vertex* vertex = polygon.GetAnyVertex();
  res.a = vertex->point;
  vertex = vertex->next;
  res.b = vertex->point;
  vertex = vertex->next;
  res.c = vertex->point;
  return res;
}

}  // namespace

// Algorithm is based on monotone triangulation
// https://neerc.ifmo.ru/wiki/index.php?title=Триангуляция_полигонов_(ушная_%2B_монотонная)
// The main idea:
//   1. decomosing given polygon to y-monotone polygons (O(NlogN))
//   2. greedily triangulating each y-monotone polygon (O(N))
std::vector<Triangle2D> Triangulate(const std::vector<Point2D>& polygon_v) {
  if (polygon_v.size() < 3)
    return {};
  Polygon2D polygon(polygon_v);
  std::list<Polygon2D> simple_polygons = ResolveIntersections(polygon);
  std::vector<Triangle2D> triangles;
  for (const Polygon2D& simple_polygon : simple_polygons) {
    std::list<Polygon2D> y_monotones = DecomposeToYMonotones(AsVector(simple_polygon));
    for (const Polygon2D& y_monotone : y_monotones)
      for (const Polygon2D& triangle_polygon : TriangulateYMonotone(y_monotone)) {
        std::optional<Triangle2D> triangle = AsTriangle(triangle_polygon);
        if (triangle)
          triangles.push_back(triangle.value());
      }
  }
  return triangles;
}

}  //geom
