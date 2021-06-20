#ifndef DCEL_POLYGON2D_H
#define DCEL_POLYGON2D_H

#include <geom_utils.h>
#include <polygon2d.h>

#include <list>
#include <optional>
#include <set>
#include <tuple>

// Doubly-connected edge list polygon representation
// Used for inserting edges and decomposing polygons

namespace geom {

struct Vertex;

struct HalfEdge {
  const Vertex* origin;
  const double angle;
  mutable const HalfEdge* prev;
  mutable const HalfEdge* next;
  mutable const HalfEdge* twin;

  HalfEdge(const Vertex* origin, const Vector2D& v);
};

struct Vertex {
  struct HalfEdgeAngleComparator {
    bool operator()(const HalfEdge* const& lhp,
                    const HalfEdge* const& rhp) const {
      if (DoubleEqual(lhp->angle, rhp->angle))
        return false;
      return lhp->angle < rhp->angle;
    }
  };

  const Point2D point;
  mutable std::set<const HalfEdge*, HalfEdgeAngleComparator> edges;

  explicit Vertex(const Point2D& point) : point(point) {}

  std::tuple<const HalfEdge*,  const HalfEdge*> GetNeighbourHalfEdges(
      const HalfEdge* edge) const;
};

bool operator<(const Vertex& lhv, const Vertex& rhv);

struct Face {
  const HalfEdge* edge;

  Face() {}
  explicit Face(const HalfEdge* edge) : edge(edge) {}
};

bool operator==(const Face& lhf, const Face& rhf);
bool operator!=(const Face& lhf, const Face& rhf);

class DcelPolygon2D {
 public:
  DcelPolygon2D(const Polygon2D& polygon2D);

  // TODO: InsertEdge and ResolveIntersection create redundant faces
  //   Asymptotically it's ok but it whould useful to find a way
  //   to maintain the correct faces list all the time

  void InsertEdge(const Segment2D& edge);
  void ResolveIntersection(const Segment2D& a, const Segment2D& b);
  std::list<Polygon2D> GetPolygons() const;

 private:
  std::optional<const HalfEdge*> GetHalfEdge(
      const Vertex* a, const Vertex* b) const;

  std::list<Face> faces_;
  std::list<HalfEdge> half_edges_;
  std::set<Vertex> vertices_;
};

}  // geom

#endif  // DCEL_POLYGON2D_H
