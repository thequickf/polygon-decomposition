#include <geom_utils.h>

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
  return v.x*u.y - v.y*u.x < 0;
}

bool operator<(const Segment2D& lhs, const Segment2D& rhs) {
  return std::tie(lhs.a, lhs.b) < std::tie(rhs.a, rhs.b);
}

bool YFirstPoint2DComparator::operator()(const Point2D& lhp,
                                         const Point2D& rhp) const {
  if (lhp.y == rhp.y)
    return lhp.x > rhp.x;
  return lhp.y < rhp.y;
}

}  // geom
