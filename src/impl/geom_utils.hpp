#ifndef IMPL_GEOM_UTILS_HPP
#define IMPL_GEOM_UTILS_HPP

#include <tuple>

namespace geom {

struct Point2D {
  double x, y;
  Point2D() : x(0), y(0) {}
  Point2D(double x, double y) : x(x), y(y) {}
};

inline bool operator<(const Point2D& lhp, const Point2D& rhp) {
  return std::tie(lhp.x, lhp.y) < std::tie(rhp.x, rhp.y);
}

inline bool operator!=(const Point2D& lhp, const Point2D& rhp) {
  return lhp < rhp || rhp < lhp;
}

inline bool operator==(const Point2D& lhp, const Point2D& rhp) {
  return !(rhp != lhp);
}

struct Vector2D {
  double x, y;
  Vector2D() : x(0), y(0) {}
  Vector2D(const Point2D& a, const Point2D& b) : x(b.x - a.x), y(b.y - a.y) {}
  Vector2D(double x, double y) : x(x), y(y) {}
};

inline bool MoreThenPiAngle2D(const Vector2D& v, const Vector2D& u) {
  return v.x*u.y - v.y*u.x < 0;
}

struct Segment2D {
  Point2D a, b;
  Segment2D(const Point2D& a, const Point2D& b) : a(a), b(b) {}
};

inline bool operator<(const Segment2D& lhs, const Segment2D& rhs) {
  return std::tie(lhs.a, lhs.b) < std::tie(rhs.a, rhs.b);
}

struct YFirstPoint2DComparator {
  bool operator()(const Point2D& lhp, const Point2D& rhp) const {
    if (lhp.y == rhp.y)
      return lhp.x > rhp.x;
    return lhp.y < rhp.y;
  }
};

}  // geom

#endif  // IMPL_GEOM_UTILS_HPP
