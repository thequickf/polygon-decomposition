#ifndef POLYGON2D_H
#define POLYGON2D_H

#include <geom_utils.h>

#include <map>
#include <optional>
#include <set>
#include <vector>

namespace geom {

class Polygon2D {
 public:
  enum PointType {
    START,
    END,
    SPLIT,
    MERGE,
    LEFT_REGULAR,
    RIGHT_REGULAR
  };

  explicit Polygon2D(std::vector<Point2D> points);

  std::optional<Point2D> Prev(const Point2D& point) const;
  std::optional<Point2D> Next(const Point2D& point) const;
  size_t Size() const;

  std::optional<Point2D> GetAnyPoint() const;
  std::optional<PointType> GetPointType(const Point2D& point) const;

 private:
  void ReverseDirection();
  bool IsClockwise() const;
  void NormalizeDirection();

  std::map<Point2D, Point2D> prev_;
  std::map<Point2D, Point2D> next_;
  std::set<Point2D> points_;
};

std::vector<Point2D> AsVector(const Polygon2D& polygon);

}  // geom

#endif  // POLYGON2D_H
