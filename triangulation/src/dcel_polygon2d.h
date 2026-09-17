#ifndef DCEL_POLYGON2D_H
#define DCEL_POLYGON2D_H

#include <geom_utils.h>
#include <polygon2d.h>

#include <array>
#include <deque>
#include <list>
#include <optional>
#include <set>
#include <tuple>

namespace geom {

// Doubly-connected edge list polygon representation
// Used for inserting edges and decomposing polygons

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
  struct Vertex;

  struct HalfEdge {
    const Vertex* origin;
    const double angle;
    mutable const HalfEdge* prev;
    mutable const HalfEdge* next;
    mutable const HalfEdge* twin;

    HalfEdge(const Vertex* origin, const Vector2D& v);

    void Visit() const { visited_ = true; }
    bool IsVisited() const { return visited_; }

   private:
    mutable bool visited_;
  };

  struct HalfEdgeAngleComparator {
    bool operator()(const HalfEdge* const& lhp,
                    const HalfEdge* const& rhp) const {
      if (DoubleEqual(lhp->angle, rhp->angle))
        return false;
      return lhp->angle < rhp->angle;
    }
  };

  class HybridEdgeSet {
   public:
    void Insert(const HalfEdge* edge);
    std::tuple<const HalfEdge*, const HalfEdge*> GetNeighbours(
        const HalfEdge* edge) const;
    const HalfEdge* LowerBound(const HalfEdge* search_key) const;

   private:
    static constexpr size_t kSmallCapacity = 4;

    void SpillToTree();

    bool using_tree_ = false;
    std::array<const HalfEdge*, kSmallCapacity> small_;
    size_t small_size_ = 0;
    std::set<const HalfEdge*, HalfEdgeAngleComparator> tree_;
  };

  struct Vertex {
    const Point2D point;
    mutable HybridEdgeSet edges;

    explicit Vertex(const Point2D& point) : point(point) {}

    std::tuple<const HalfEdge*, const HalfEdge*> GetNeighbourHalfEdges(
        const HalfEdge* edge) const {
      return edges.GetNeighbours(edge);
    }
  };

  struct Face {
    const HalfEdge* edge;

    Face() {}
    explicit Face(const HalfEdge* edge) : edge(edge) {}
  };

  friend bool operator<(const Vertex& lhv, const Vertex& rhv);

  friend bool operator==(const Face& lhf, const Face& rhf);
  friend bool operator!=(const Face& lhf, const Face& rhf);

  std::optional<const HalfEdge*> GetHalfEdge(
      const Vertex* a, const Vertex* b) const;

  std::deque<Face> faces_;
  std::deque<HalfEdge> half_edges_;
  std::set<Vertex> vertices_;
};

}  // geom

#endif  // DCEL_POLYGON2D_H
