#ifndef TRIAGULATION_EXPOSE_TRIANGULATION_BASE_GEOMETRY_H
#define TRIAGULATION_EXPOSE_TRIANGULATION_BASE_GEOMETRY_H

namespace geom {

struct Point2D {
  double x, y;
  Point2D() : x(0), y(0) {}
  Point2D(double x, double y) : x(x), y(y) {}
};

struct Triangle2D {
  Point2D a, b, c;
  Triangle2D() {}
  Triangle2D(const Point2D& a, const Point2D& b, const Point2D& c) :
      a(a), b(b), c(c) {}
};

}  // geom

#endif  // TRIAGULATION_EXPOSE_TRIANGULATION_BASE_GEOMETRY_H
