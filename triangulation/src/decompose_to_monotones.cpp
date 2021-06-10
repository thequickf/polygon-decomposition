#include <decompose_to_monotones.h>

#include <dcel_polygon2d.h>

#include <algorithm>
#include <cassert>
#include <set>

namespace geom {

namespace {

static double sweep_line_y;

struct LeftEdgesComparator {
  double XAtSweepLine(const Segment2D& segment) const {
    Vector2D v = {segment.a, segment.b};
    if (v.y == 0)
      return segment.a.x;
    double k = (sweep_line_y - segment.a.y) / (v.y);
    return segment.a.x + v.x * k;
  }

  bool operator()(const Segment2D& lhs, const Segment2D& rhs) const {
    return XAtSweepLine(lhs) < XAtSweepLine(rhs);
  }
};

std::optional<Segment2D> FindFirstLeftEdgeToPoint(
    const std::set<Segment2D, LeftEdgesComparator>& left_edges,
    const Point2D& point) {
  auto it = left_edges.upper_bound({point, point});
  it--;
  if (it != left_edges.end())
    return *it;
  assert(false);
  return {};
}

}  // namespace

// Decomposing to y-montones is quite complicated
// (Probably implementation is messy)
// Please check the link in triangulation.cpp to get some understanding
std::list<Polygon2D> DecomposeToYMonotones(
    const std::vector<Point2D>& polygon_v) {
  Polygon2D polygon(polygon_v);
  DcelPolygon2D dcel_polygon(polygon);
  std::vector<Point2D> points(polygon_v);
  std::sort(points.rbegin(), points.rend(), YFirstPoint2DComparator());
  std::set<Segment2D, LeftEdgesComparator> left_edges;
  std::map<Segment2D, Point2D> y_min_points;
  for (const Point2D& point : points) {
    sweep_line_y = point.y;
    switch (polygon.GetPointType(point).value()) {
      case Polygon2D::START: {
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        left_edges.insert(prev_edge);
        y_min_points[prev_edge] = point;
      } break;
      case Polygon2D::END: {
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Point2D next_y_min_point = y_min_points[next_edge];
        if (polygon.GetPointType(next_y_min_point).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, next_y_min_point});
        left_edges.erase(next_edge);
      } break;
      case Polygon2D::SPLIT: {
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        left_edges.insert(prev_edge);
        y_min_points[prev_edge] = point;
        if (!left_edge)
          break;
        dcel_polygon.InsertEdge({point, y_min_points[left_edge.value()]});
        y_min_points[left_edge.value()] = point;
      } break;
      case Polygon2D::MERGE: {
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Point2D next_y_min_point = y_min_points[next_edge];
        if (polygon.GetPointType(next_y_min_point).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, next_y_min_point});
        left_edges.erase(next_edge);
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        if (!left_edge)
          break;
        Point2D left_edge_helper = y_min_points[left_edge.value()];
        if (polygon.GetPointType(left_edge_helper).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, left_edge_helper});
        y_min_points[left_edge.value()] = point;
      } break;
      case Polygon2D::LEFT_REGULAR: {
        Segment2D next_edge = {polygon.Next(point).value(), point};
        Segment2D prev_edge = {point, polygon.Prev(point).value()};
        if (polygon.GetPointType(y_min_points[next_edge]) == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, y_min_points[next_edge]});
        left_edges.erase(next_edge);
        left_edges.insert(prev_edge);
        y_min_points[prev_edge] = point;
      } break;
      case Polygon2D::RIGHT_REGULAR: {
        std::optional<Segment2D> left_edge =
            FindFirstLeftEdgeToPoint(left_edges, point);
        if (!left_edge)
          break;
        Point2D y_min_point = y_min_points[left_edge.value()];
        if (polygon.GetPointType(y_min_point).value() == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({point, y_min_point});
        y_min_points[left_edge.value()] = point;
      } break;
    }
  }
  return dcel_polygon.GetPolygons();
}

}  // geom
