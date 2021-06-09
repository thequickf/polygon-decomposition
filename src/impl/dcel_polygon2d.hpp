#ifndef DCEL_POLYGON2D_HPP
#define DCEL_POLYGON2D_HPP

#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>

#include <cmath>
#include <map>
#include <optional>
#include <set>
#include <tuple>
#include <list>

namespace geom {

struct Vertex;

struct HalfEdge {
  const Vertex* origin;
  const double angle;
  mutable HalfEdge* prev;
  mutable HalfEdge* next;
  mutable HalfEdge* twin;

  HalfEdge(const Vertex* origin, const Vector2D& v) :
      origin(origin), angle(std::atan2(v.y, v.x)) {}
};

struct Vertex {
  struct HalfEdgeAngleComparator {
    bool operator()(HalfEdge* const& lhp, HalfEdge* const& rhp) const {
      return lhp->angle < rhp->angle;
    }
  };

  const Point2D point;
  mutable std::set<HalfEdge*, HalfEdgeAngleComparator> edges;

  explicit Vertex(const Point2D& point) : point(point) {}

  std::tuple<HalfEdge*, HalfEdge*> GetNeighbourHalfEdges(
      HalfEdge* edge) const {
    auto right = edges.upper_bound(edge);
    if (right == edges.end())
      right = edges.begin();

    auto left = edges.upper_bound(edge);
    if (left == edges.begin())
      left = edges.end();
    left--;
    
    return std::make_tuple(*left, *right);
  }
};

inline bool operator<(const Vertex& lhv, const Vertex& rhv) {
  return lhv.point < rhv.point;
}

struct Face {
  HalfEdge* edge;

  explicit Face(HalfEdge* edge) : edge(edge) {}
};

class DcelPolygon2D {
 public:
  DcelPolygon2D(const Polygon2D& polygon2D) {
    std::map<Point2D, const Vertex*> pnt_to_vertex;
    std::map<Point2D, HalfEdge*> pnt_to_edge_forward;

    Point2D current = polygon2D.GetAnyPoint().value();
    for (size_t i = 0; i < polygon2D.Size(); i++) {
      Point2D next = polygon2D.Next(current).value();

      const Vertex* vertex = &*vertices_.insert(Vertex(current)).first;
      pnt_to_vertex[current] = vertex;

      half_edges_.push_back(HalfEdge(vertex, {current, next}));
      HalfEdge* edge = &*half_edges_.rbegin();
      vertex->edges.insert(edge);
      pnt_to_edge_forward[current] = edge;

      current = next;
    }

    for (size_t i = 0; i < polygon2D.Size(); i++) {
      Point2D prev = polygon2D.Prev(current).value();
      Point2D next = polygon2D.Next(current).value();

      HalfEdge* edge = pnt_to_edge_forward[current];
      edge->prev = pnt_to_edge_forward[prev];
      edge->next = pnt_to_edge_forward[next];

      current = next;
    }

    faces_.push_back(Face(pnt_to_edge_forward[current]));

    std::map<Point2D, HalfEdge*> pnt_to_edge_back;
    for (size_t i = 0; i < polygon2D.Size(); i++) {
      Point2D next = polygon2D.Next(current).value();

      const Vertex* vertex = pnt_to_vertex[next];

      half_edges_.push_back(HalfEdge(vertex, {next, current}));
      HalfEdge* edge = &*half_edges_.rbegin();
      vertex->edges.insert(edge);

      pnt_to_edge_forward[current]->twin = edge;
      edge->twin = pnt_to_edge_forward[current];

      pnt_to_edge_back[next] = edge;

      current = next;
    }

    for (size_t i = 0; i < polygon2D.Size(); i++) {
      Point2D prev = polygon2D.Prev(current).value();
      Point2D next = polygon2D.Next(current).value();

      HalfEdge* edge = pnt_to_edge_back[current];
      edge->prev = pnt_to_edge_back[next];
      edge->next = pnt_to_edge_back[prev];

      current = next;
    }
  }

  void InsertEdge(const Segment2D& edge) {
    auto u_it = vertices_.find(Vertex(edge.a));
    auto v_it = vertices_.find(Vertex(edge.b));
    if (u_it == vertices_.end() || v_it == vertices_.end())
      return;

    const Vertex* u = &*u_it;
    const Vertex* v = &*v_it;

    half_edges_.push_back(HalfEdge(u, {edge.a, edge.b}));
    HalfEdge* uv_edge = &*half_edges_.rbegin();
    half_edges_.push_back(HalfEdge(v, {edge.b, edge.a}));
    HalfEdge* vu_edge = &*half_edges_.rbegin();

    uv_edge->twin = vu_edge;
    vu_edge->twin = uv_edge;

    HalfEdge* u_left;
    HalfEdge* u_right;
    HalfEdge* v_left;
    HalfEdge* v_right;
    std::tie(u_left, u_right) = u->GetNeighbourHalfEdges(uv_edge);
    std::tie(v_left, v_right) = v->GetNeighbourHalfEdges(vu_edge);
    
    u_left->twin->next = uv_edge;
    uv_edge->prev = u_left->twin;
    uv_edge->next = v_right;
    v_right->prev = uv_edge;

    v_left->twin->next = vu_edge;
    vu_edge->prev = v_left->twin;
    vu_edge->next = u_right;
    u_right->prev = vu_edge;

    u->edges.insert(uv_edge);
    v->edges.insert(vu_edge);

    faces_.push_back(Face(vu_edge));
    faces_.push_back(Face(uv_edge));
  }

  std::list<Polygon2D> GetPolygons() const {
    std::list<Polygon2D> res;

    std::set<const HalfEdge*> visited;
    for (const Face& face : faces_) {
      HalfEdge* start_edge = face.edge;
      if (visited.count(start_edge))
        continue;
      std::vector<Point2D> polygon_vector;
      HalfEdge* edge = face.edge;
      do {
        polygon_vector.push_back(edge->origin->point);
        visited.insert(edge);

        edge = edge->next;
      } while (edge != start_edge);

      res.push_back(Polygon2D(polygon_vector));
    }

    return res;
  }

 private:
  std::list<Face> faces_;
  std::list<HalfEdge> half_edges_;
  std::set<Vertex> vertices_;
};

}  // geom

#endif  // DCEL_POLYGON2D_HPP
