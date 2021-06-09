#ifndef GEOM_UTILS_H
#define GEOM_UTILS_H

#include <triangulation_base_geometry.h>

namespace geom {

struct Vector2D {
  double x, y;
  Vector2D() : x(0), y(0) {}
  Vector2D(const Point2D& a, const Point2D& b) : x(b.x - a.x), y(b.y - a.y) {}
  Vector2D(double x, double y) : x(x), y(y) {}
};

struct Segment2D {
  Point2D a, b;
  Segment2D(const Point2D& a, const Point2D& b) : a(a), b(b) {}
};

struct YFirstPoint2DComparator {
  bool operator()(const Point2D& lhp, const Point2D& rhp) const;
};

bool operator<(const Point2D& lhp, const Point2D& rhp);

bool operator!=(const Point2D& lhp, const Point2D& rhp);

bool operator==(const Point2D& lhp, const Point2D& rhp);

bool MoreThenPiAngle2D(const Vector2D& v, const Vector2D& u);

bool operator<(const Segment2D& lhs, const Segment2D& rhs);

}  // geom

#endif  // GEOM_UTILS_H
