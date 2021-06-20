#include <decompose_to_monotones.h>

#include <dcel_polygon2d.h>
#include <segments_on_y_sweep_line.h>

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <set>

namespace geom {

// Decomposing to y-montones is quite complicated
// (Probably implementation is messy)
// Please check the link in triangulation.cpp to get some understanding
std::list<Polygon2D> DecomposeToYMonotones(
    const std::vector<Point2D>& polygon_v) {
  const Polygon2D polygon(polygon_v);
  DcelPolygon2D dcel_polygon(polygon);
  std::vector<const Polygon2D::Vertex*> vertices = AsVertexVector(polygon);
  std::sort(vertices.rbegin(), vertices.rend(), YFirstVertexComparator());
  SegmentsOnYSweepLine left_edges;
  std::unordered_map<Segment2D, const Polygon2D::Vertex*> y_min_vertices;
  for (const Polygon2D::Vertex* vertex : vertices) {
    SegmentsOnYSweepLine::SetY(vertex->point.y);
    switch (vertex->type) {
      case Polygon2D::START: {
        const Segment2D prev_edge = {vertex->point, vertex->prev->point};
        left_edges.Add(prev_edge);
        y_min_vertices[prev_edge] = vertex;
        break;
      }
      case Polygon2D::END: {
        const Segment2D next_edge = {vertex->next->point, vertex->point};
        const Polygon2D::Vertex* next_y_min_vertex = y_min_vertices[next_edge];
        if (next_y_min_vertex->type == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({vertex->point, next_y_min_vertex->point});
        left_edges.Remove(next_edge);
        break;
      }
      case Polygon2D::SPLIT: {
        const Segment2D prev_edge = {vertex->point, vertex->prev->point};
        const std::optional<Segment2D> left_edge =
          left_edges.FirstLeft(vertex->point);
        left_edges.Add(prev_edge);
        y_min_vertices[prev_edge] = vertex;
        if (!left_edge)
          break;
        const Polygon2D::Vertex* left_edge_y_min_vertex =
            y_min_vertices[left_edge.value()];
        dcel_polygon.InsertEdge({vertex->point, left_edge_y_min_vertex->point});
        y_min_vertices[left_edge.value()] = vertex;
        break;
      }
      case Polygon2D::MERGE: {
        const Segment2D next_edge = {vertex->next->point, vertex->point};
        const Polygon2D::Vertex* next_y_min_vertex = y_min_vertices[next_edge];
        if (next_y_min_vertex->type == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({vertex->point, next_y_min_vertex->point});
        left_edges.Remove(next_edge);
        const std::optional<Segment2D> left_edge =
          left_edges.FirstLeft(vertex->point);
        if (!left_edge)
          break;
        const Polygon2D::Vertex* left_edge_y_min_vertex =
            y_min_vertices[left_edge.value()];
        if (left_edge_y_min_vertex->type == Polygon2D::MERGE)
          dcel_polygon.InsertEdge(
              {vertex->point, left_edge_y_min_vertex->point});
        y_min_vertices[left_edge.value()] = vertex;
        break;
      }
      case Polygon2D::LEFT_REGULAR: {
        const Segment2D next_edge = {vertex->next->point, vertex->point};
        const Segment2D prev_edge = {vertex->point, vertex->prev->point};
        if (y_min_vertices[next_edge]->type == Polygon2D::MERGE)
          dcel_polygon.InsertEdge(
              {vertex->point, y_min_vertices[next_edge]->point});
        left_edges.Remove(next_edge);
        left_edges.Add(prev_edge);
        y_min_vertices[prev_edge] = vertex;
        break;
      }
      case Polygon2D::RIGHT_REGULAR: {
        const std::optional<Segment2D> left_edge =
          left_edges.FirstLeft(vertex->point);
        if (!left_edge)
          break;
        const Polygon2D::Vertex* y_min_vertex =
            y_min_vertices[left_edge.value()];
        if (y_min_vertex->type == Polygon2D::MERGE)
          dcel_polygon.InsertEdge({vertex->point, y_min_vertex->point});
        y_min_vertices[left_edge.value()] = vertex;
        break;
      }
    }
  }
  return dcel_polygon.GetPolygons();
}

}  // geom
