#include <geom_utils.h>

#include <cassert>
#include <climits>
#include <cmath>
#include <tuple>

namespace geom {

bool operator<(const Point2D& lhp, const Point2D& rhp) {
  return std::tie(lhp.x, lhp.y) < std::tie(rhp.x, rhp.y);
}

bool operator!=(const Point2D& lhp, const Point2D& rhp) {
  return lhp < rhp || rhp < lhp;
}

bool operator==(const Point2D& lhp, const Point2D& rhp) {
  return !(rhp != lhp);
}

bool MoreThenPiAngle2D(const Vector2D& v, const Vector2D& u) {
  const double z = v.x*u.y - v.y*u.x;
  if (DoubleEqual(z, 0))
    return false;
  return z < 0;
}

bool operator<(const Segment2D& lhs, const Segment2D& rhs) {
  return std::tie(lhs.a, lhs.b) < std::tie(rhs.a, rhs.b);
}

bool operator==(const Segment2D& lhs, const Segment2D& rhs) {
  return std::tie(lhs.a, lhs.b) == std::tie(rhs.a, rhs.b);
}

bool YFirstPoint2DComparator::operator()(const Point2D& lhp,
                                         const Point2D& rhp) const {
  if (DoubleEqual(lhp, rhp))
    return false;
  if (DoubleEqual(lhp.y, rhp.y))
    return lhp.x > rhp.x;
  return lhp.y < rhp.y;
}

bool DoubleEqual(double lhd, double rhd) {
  return std::abs(lhd - rhd) < 1e-10;
}

bool DoubleLessOrEqual(double lhd, double rhd) {
  return DoubleEqual(lhd, rhd) || lhd < rhd;
}

bool DoubleEqual(const geom::Point2D& lhp, const geom::Point2D& rhp) {
  return geom::DoubleEqual(lhp.x, rhp.x) && geom::DoubleEqual(lhp.y, rhp.y);
}

bool DoubleEqual(const geom::Segment2D& lhs, const geom::Segment2D& rhs) {
  return geom::DoubleEqual(lhs.a, rhs.a) && geom::DoubleEqual(lhs.b, rhs.b);
}

// assuming that vectors are collinear
double MagnitudeRatio(const Vector2D& v, const Vector2D& u) {
  const double v_sqr = v.x*v.x + v.y*v.y;
  const double v_u = v.x*u.x + v.y*u.y;
  return v_sqr / v_u;
}

// assuming that the segment and the point are on the same line
bool IsPointInsideSegment(const Segment2D& segment, const Point2D& point) {
  if (DoubleEqual(segment.a, point) || DoubleEqual(segment.a, segment.b))
    return DoubleEqual(segment.a, point);
  const double k = MagnitudeRatio({segment.a, point}, {segment.a, segment.b});
  return DoubleLessOrEqual(0, k) && DoubleLessOrEqual(k, 1);
}

std::optional<Point2D> IntersectionPoint(const Segment2D& a,
                                         const Segment2D& b) {
  const Vector2D v1 = {a.a, a.b};
  const Vector2D v2 = {b.a, b.b};
  const double s1 = v1.x*v2.y - v1.y*v2.x;
  const Vector2D v3 = {a.a, b.a};
  const double s2 = v1.x*v3.y - v1.y*v3.x;
  if (DoubleEqual(s1, 0)) {
    if (DoubleEqual(s2, 0)) {
      // On the same line
      if (IsPointInsideSegment(a, b.a))
        return b.a;
      if (IsPointInsideSegment(a, b.b))
        return b.b;
      return {};
    }
    // Parallel
    return {};
  }
  const double s3 = v3.x*v2.y - v3.y*v2.x;
  const double k = s3/s1;
  const Point2D line_intersection_point = {a.a.x + v1.x * k, a.a.y + v1.y * k};
  if (IsPointInsideSegment(a, line_intersection_point) &&
      IsPointInsideSegment(b, line_intersection_point))
    return line_intersection_point;
  return {};
}

bool IsIntersectionOnVertex(const Segment2D& a, const Segment2D& b) {
  return DoubleEqual(a.a, b.a) || DoubleEqual(a.a, b.b) ||
         DoubleEqual(a.b, b.a) || DoubleEqual(a.b, b.b);
}

std::size_t CombineHash(std::size_t left, std::size_t right) {
  return left ^ (right << 1 | (right >> (CHAR_BIT * sizeof(right) - 1)));
}

}  // geom
