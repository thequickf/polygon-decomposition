#include <triangulate_monotone.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>

#include <algorithm>
#include <stack>
#include <vector>

namespace geom {

namespace {

bool IsAdjacent(const Polygon2D& polygon,
    const Point2D& a, const Point2D& b) {
  return polygon.Prev(a) == b || polygon.Next(a) == b;
}

bool IsValidDiagonal(const Polygon2D& polygon,
    const Point2D& current, const Point2D& last, const Point2D& stk_peek) {
  Vector2D v = {current, last};
  Vector2D u = {current, stk_peek};
  Polygon2D::PointType type = polygon.GetPointType(current).value();
  bool right = MoreThenPiAngle2D(u, v);
  return (type == Polygon2D::RIGHT_REGULAR) == right;
}

}  // namespace

std::list<Polygon2D> TriangulateYMonotone(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);
  std::vector<Point2D> polygon_v = AsVector(polygon);
  std::sort(polygon_v.rbegin(), polygon_v.rend(), YFirstPoint2DComparator());
  std::stack<Point2D> to_process_stk;
  to_process_stk.push(polygon_v[0]);
  to_process_stk.push(polygon_v[1]);
  size_t i = 2;
  for (; i < polygon_v.size() - 1; i++) {
    if (IsAdjacent(polygon, polygon_v[i], to_process_stk.top())) {
      Point2D last = to_process_stk.top();
      to_process_stk.pop();
      while (to_process_stk.size() > 0 &&
          IsValidDiagonal(polygon, polygon_v[i], last, to_process_stk.top())) {
        last = to_process_stk.top();
        to_process_stk.pop();
        dcel_polygon.InsertEdge({polygon_v[i], last});
      }
      to_process_stk.push(last);
      to_process_stk.push(polygon_v[i]);
    } else {
      while (to_process_stk.size() > 0) {
        if (to_process_stk.size() != 1)
          dcel_polygon.InsertEdge({polygon_v[i], to_process_stk.top()});
        to_process_stk.pop();
      }
      to_process_stk.push(polygon_v[i - 1]);
      to_process_stk.push(polygon_v[i]);
    }
  }
  to_process_stk.pop();
  while (to_process_stk.size() > 0) {
    if (to_process_stk.size() != 1)
      dcel_polygon.InsertEdge({polygon_v[i], to_process_stk.top()});
    to_process_stk.pop();
  }
  return dcel_polygon.GetPolygons();
}

}  // geom
