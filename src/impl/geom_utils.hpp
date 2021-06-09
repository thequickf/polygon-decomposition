#ifndef IMPL_GEOM_UTILS_HPP
#define IMPL_GEOM_UTILS_HPP

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

struct Point3D : public Point2D {
  double z;
  Point3D() : Point2D(), z(0) {}
  Point3D(double x, double y, double z) : Point2D(x, y), z(z) {}
};

inline bool operator<(const Point3D& lhp, const Point3D& rhp) {
  return std::tie(lhp.x, lhp.y, lhp.z) < std::tie(rhp.x, rhp.y, rhp.z);
}

struct Vector2D {
  double x, y;
  Vector2D() : x(0), y(0) {}
  Vector2D(const Point2D& a, const Point2D& b) : x(b.x - a.x), y(b.y - a.y) {}
  Vector2D(double x, double y) : x(x), y(y) {}
};

struct Vector3D : public Vector2D {
  double z;
  Vector3D() : Vector2D(), z(0) {}
  Vector3D(const Point3D& a, const Point3D& b) :
      Vector2D(b.x - a.x, b.y - a.y), z(b.z - a.z) {}
  Vector3D(double x, double y, double z) : Vector2D(x, y), z(z) {}
};

inline Point3D Convert3D(Point2D p, double z = 0) {
  return {p.x, p.y, z};
}

inline Vector3D Convert3D(Vector2D v, double z = 0) {
  return {v.x, v.y, z};
}

inline Point2D Convert2D(Point3D p) {
  return {p.x, p.y};
}

inline Vector2D Convert2D(Vector3D v) {
  return {v.x, v.y};
}

inline double ScalarProduct(const Vector2D& v, const Vector2D& u) {
  return v.x*u.x + v.y*u.y;
}

inline double ScalarProduct(const Vector3D& v, const Vector3D& u) {
  return v.x*u.x + v.y*u.y + v.z*u.z;
}

inline Vector2D CrossProduct(const Vector2D& v, const Vector2D& u) {
  return {0, 0};
}

inline Vector3D CrossProduct(const Vector3D& v, const Vector3D& u) {
  return {v.y*u.z - v.z*u.y, v.z*u.x - v.x*u.z,  v.x*u.y - v.y*u.x};
}

inline bool MoreThenPiAngle2D(const Vector2D& v, const Vector2D& u) {
  return CrossProduct(Convert3D(v), Convert3D(u)).z < 0;
}

struct Segment2D {
  Point2D a, b;
  Segment2D(const Point2D& a, const Point2D& b) : a(a), b(b) {}
};

inline bool operator<(const Segment2D& lhs, const Segment2D& rhs) {
  return std::tie(lhs.a, lhs.b) < std::tie(rhs.a, rhs.b);
}

struct Triangle2D : public Segment2D {
  Point2D c;
  Triangle2D(const Point2D& a, const Point2D& b, const Point2D& c) :
      Segment2D(a, b), c(c) {}
};

struct YFirstPoint2DComparator {
  bool operator()(const Point2D& lhp, const Point2D& rhp) {
    if (lhp.y == rhp.y)
      return lhp.x > rhp.x;
    return lhp.y < rhp.y;
  }
};

}  // geom

#endif  // IMPL_GEOM_UTILS_HPP
