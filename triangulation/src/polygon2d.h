#ifndef POLYGON2D_H
#define POLYGON2D_H

#include <geom_utils.h>

#include <functional>
#include <list>
#include <vector>

namespace geom {

class Polygon2D {
 public:
  enum VertexType {
    START,
    END,
    SPLIT,
    MERGE,
    LEFT_REGULAR,
    RIGHT_REGULAR
  };

  struct Vertex {
    const Point2D point;
    Vertex* prev;
    Vertex* next;
    VertexType type;

    explicit Vertex(const Point2D& point) : point(point) {}
  };

  explicit Polygon2D(const std::vector<Point2D>& points);

  size_t Size() const;

  const Vertex* GetAnyVertex() const;

 private:
  static VertexType GetVertexType(const Vertex* vertex);
  void SetVertexTypes();

  void ReverseDirection();
  bool IsClockwise() const;
  void NormalizeDirection();

  std::list<Vertex> vertices_;
};

struct YFirstVertexComparator {
  bool operator()(const Polygon2D::Vertex* const& lhv,
                  const Polygon2D::Vertex* const& rhv) const;
};

std::vector<Point2D> AsVector(const Polygon2D& polygon);
std::vector<const Polygon2D::Vertex*> AsVertexVector(const Polygon2D& polygon);

}  // geom

#endif  // POLYGON2D_H
