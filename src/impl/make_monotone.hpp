#ifndef IMPL_MAKE_MONOTONE_HPP
#define IMPL_MAKE_MONOTONE_HPP

#include <impl/dcel_polygon2d.hpp>
#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>

#include <algorithm>
#include <cassert>

namespace geom {

namespace {

// assuming that second point of segment and given point are more or
// equal by YFirstPoint2DComparator then first point of segment
inline bool IsLeftToPoint(const Segment2D& segment, const Point2D& point) {
  return MoreThenPiAngle2D({segment.a, point}, {segment.a, segment.b});
}

inline std::optional<Segment2D> FindFirstLeftEdgeToPoint(
    const std::set<Segment2D> left_edges, const Point2D& point) {
  auto it = left_edges.upper_bound({point, point});
  if (it != left_edges.end() && IsLeftToPoint(*it, point))
    return *it;
  it--;
  if (it != left_edges.end() && IsLeftToPoint(*it, point))
    return *it;
  assert(false);
  return {};
}

}  // namespace

// TODO: This function is a mess. Clean it up.
inline std::list<Polygon2D> DecomposeToYMonotones(
    const std::vector<Point2D>& polygon_v) {
  Polygon2D polygon(polygon_v);
  DcelPolygon2D dcel_polygon(polygon);
  std::vector<Point2D> points(polygon_v);
  std::sort(points.rbegin(), points.rend(), YFirstPoint2DComparator());
  std::set<Segment2D> left_edges;
  std::map<Segment2D, Point2D> helper;
  for (const Point2D& point : points) {
    switch (polygon.GetPointType(point).value()) {
      case Polygon2D::START: {
        // Handle Start Point
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        left_edges.insert(prev_edge);
        helper[prev_edge] = point;
      } break;
      case Polygon2D::END: {
        // Handle End Point
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Point2D next_helper = helper[next_edge];
        if (polygon.GetPointType(next_helper).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, next_helper});
        left_edges.erase(next_edge);
      } break;
      case Polygon2D::SPLIT: {
        // Handle Split Point
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        left_edges.insert(prev_edge);
        helper[prev_edge] = point;
        if (!left_edge)
          break;
        dcel_polygon.InsertEdge({point, helper[left_edge.value()]});
        helper[left_edge.value()] = point;
      } break;
      case Polygon2D::MERGE: {
        //  Handle Merge Point
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Point2D next_helper = helper[next_edge];
        if (polygon.GetPointType(next_helper).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, next_helper});
        left_edges.erase(next_edge);
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        if (!left_edge)
          break;
        Point2D left_edge_helper = helper[left_edge.value()];
        if (polygon.GetPointType(left_edge_helper).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, left_edge_helper});
        helper[left_edge.value()] = point;
      } break;
      case Polygon2D::LEFT_REGULAR: {
        // Handle Left Regular Point
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        if (polygon.GetPointType(helper[next_edge]) == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, helper[next_edge]});
        left_edges.erase(next_edge);
        left_edges.insert(prev_edge);
        helper[prev_edge] = point;
      } break;
      case Polygon2D::RIGHT_REGULAR: {
        // Handle Right Regular Point
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        if (!left_edge)
          break;
        Point2D left_edge_helper = helper[left_edge.value()];
        if (polygon.GetPointType(left_edge_helper).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, left_edge_helper});
        helper[left_edge.value()] = point;
      } break;
    }
  }
  return dcel_polygon.GetPolygons();
}

}  // geom

#endif  // IMPL_MAKE_MONOTONE_HPP
