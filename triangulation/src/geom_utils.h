#ifndef GEOM_UTILS_H
#define GEOM_UTILS_H

#include <triangulation_base_geometry.h>

#include <optional>

namespace geom {

struct Vector2D {
  double x, y;
  Vector2D(const Point2D& a, const Point2D& b) : x(b.x - a.x), y(b.y - a.y) {}
  Vector2D(double x, double y) : x(x), y(y) {}
};

struct Segment2D {
  Point2D a, b;
  Segment2D() {}
  Segment2D(const Point2D& a, const Point2D& b) : a(a), b(b) {}
};

// First by Y, then by X in opposite order
struct YFirstPoint2DComparator {
  bool operator()(const Point2D& lhp, const Point2D& rhp) const;
};

bool operator<(const Point2D& lhp, const Point2D& rhp);
bool operator!=(const Point2D& lhp, const Point2D& rhp);
bool operator==(const Point2D& lhp, const Point2D& rhp);

bool MoreThenPiAngle2D(const Vector2D& v, const Vector2D& u);

bool operator<(const Segment2D& lhs, const Segment2D& rhs);
bool operator==(const Segment2D& lhs, const Segment2D& rhs);

bool DoubleEqual(double lhd, double rhd);
bool DoubleLessOrEqual(double lhd, double rhd);
bool DoubleEqual(const geom::Point2D& lhp, const geom::Point2D& rhp);
bool DoubleEqual(const geom::Segment2D& lhs, const geom::Segment2D& rhs);

std::optional<Point2D> IntersectionPoint(const Segment2D& a,
                                         const Segment2D& b);

struct SegmentOnSweepLineComparator {
  static double sweep_line_y;

  bool operator()(const Segment2D& lhs, const Segment2D& rhs) const;

 // private:
  static double AnyXAtSweepLine(const Segment2D& segment);
};

bool IsIntersectionOnVertex(const Segment2D& a, const Segment2D& b);

std::size_t CombineHash(std::size_t left, std::size_t right);

}  // geom

namespace std {

template<>
struct hash<geom::Point2D> {
  std::size_t operator()(const geom::Point2D& point) const noexcept {
    return geom::CombineHash(hash<double>()(point.x), hash<double>()(point.y));
  }
};

template<>
struct hash<geom::Segment2D> {
  std::size_t operator()(const geom::Segment2D& segment) const noexcept {
    return geom::CombineHash(hash<geom::Point2D>()(segment.a),
                             hash<geom::Point2D>()(segment.b));
  }
};

}  // std

#endif  // GEOM_UTILS_H
