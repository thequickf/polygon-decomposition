#ifndef IMPL_TRIANGULATE_MONOTONE_HPP
#define IMPL_TRIANGULATE_MONOTONE_HPP

#include <impl/dcel_polygon2d.hpp>
#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>

#include <stack>

namespace geom {

namespace {

inline bool IsAdjacent(const Polygon2D& polygon,
    const Point2D& a, const Point2D& b) {
  return polygon.Prev(a) == b || polygon.Next(a) == b;
}

inline bool IsValidDiagonal(const Polygon2D& polygon,
    const Point2D& current, const Point2D& last, const Point2D& stk_peek) {
  Vector2D v = {current, last};
  Vector2D u = {current, stk_peek};
  Polygon2D::PointType type = polygon.GetPointType(current).value();
  bool right = MoreThenPiAngle2D(u, v);
  return (type == Polygon2D::RIGHT_REGULAR) == right;
}

}  // namespace

inline std::list<Polygon2D> TriangulateYMonotone(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);
  std::vector<Point2D> polygon_v = AsVector(polygon);
  std::sort(polygon_v.rbegin(), polygon_v.rend(), YFirstPoint2DComparator());
  std::stack<Point2D> stk;
  stk.push(polygon_v[0]);
  stk.push(polygon_v[1]);
  size_t i = 2;
  for (; i < polygon_v.size() - 1; i++) {
    if (IsAdjacent(polygon, polygon_v[i], stk.top())) {
      Point2D last = stk.top();
      stk.pop();
      while (stk.size() > 0 &&
             IsValidDiagonal(polygon, polygon_v[i], last, stk.top())) {
        last = stk.top();
        stk.pop();
        dcel_polygon.InsertEdge({polygon_v[i], last});
      }
      stk.push(last);
      stk.push(polygon_v[i]);
    } else {
      while (stk.size() > 0) {
        if (stk.size() != 1)
          dcel_polygon.InsertEdge({polygon_v[i], stk.top()});
        stk.pop();
      }
      stk.push(polygon_v[i - 1]);
      stk.push(polygon_v[i]);
    }
  }
  stk.pop();
  while (stk.size() > 0) {
    if (stk.size() != 1)
      dcel_polygon.InsertEdge({polygon_v[i], stk.top()});
    stk.pop();
  }
  return dcel_polygon.GetPolygons();
}

}  // geom

#endif  // IMPL_TRIANGULATE_MONOTONE_HPP
