#ifndef IMPL_POLYGON2D_HPP
#define IMPL_POLYGON2D_HPP

#include <impl/geom_utils.hpp>

#include <map>
#include <optional>
#include <set>
#include <tuple>
#include <vector>

namespace geom {

class Polygon2D {
 public:
  enum PointType {
    START,
    END,
    SPLIT,
    MERGE,
    REGULAR
  };

  explicit Polygon2D(std::vector<Point2D> points) {
    if (points.size() == 0)
      return;
    
    Point2D prev = points[0];
    points_.insert(points[0]);
    for (size_t i = 1; i < points.size(); i++)
      if (points_.insert(points[i]).second) {
        prev_[points[i]] = prev;
        prev = points[i];
      }
    prev_[points[0]] = prev;

    Point2D current = GetAnyPoint().value();
    for (size_t i = 0; i < Size(); i++) {
      next_[prev_[current]] = current;
      current = prev_[current];
    }

    NormalizeDirection();
  }

  std::optional<Point2D> Prev(const Point2D& point) const {
    if (points_.count(point))
      return prev_.at(point);
    return {};
  }

  std::optional<Point2D> Next(const Point2D& point) const {
    if (points_.count(point))
      return next_.at(point);
    return {};
  }

  size_t Size() const {
    return points_.size();
  }

  std::optional<Point2D> GetAnyPoint() const {
    if (Size() > 0)
      return *points_.begin();
    return {};
  }

  std::optional<PointType> GetPointType(const Point2D& point) const {
    if (!points_.count(point))
      return {};
    const Point2D prev = Prev(point).value();
    const Point2D next = Next(point).value();
    const bool start_kind = YFirstPoint2DComparator()(prev, point) &&
                            YFirstPoint2DComparator()(next, point);
    const bool end_kind = YFirstPoint2DComparator()(point, prev) &&
                          YFirstPoint2DComparator()(point, next);
    if (start_kind || end_kind) {
      const geom::Vector2D v = {point, prev};
      const geom::Vector2D u = {point, next};
      const geom::Vector3D w =
          geom::cross_product(geom::convert3D(v), geom::convert3D(u));
      if (w.z < 0)
        if (start_kind)
          return SPLIT;
        else
          return MERGE;
      else
        if (start_kind)
          return START;
        else
          return END;
    }
    else {
      return REGULAR;
    }
  }

 private:
  void ReverseDirection() {
    if (Size() == 0)
      return;

    Point2D current = GetAnyPoint().value();
    for (size_t i = 0; i < Size(); i++) {
      const Point2D prev = Prev(current).value();
      const Point2D next = Next(current).value();
      prev_[current] = next;
      next_[current] = prev;
      current = next;
    }
  }
 
  bool IsClockwise() const  {
    Point2D current = GetAnyPoint().value();
    long double sum = 0;
    for (size_t i = 0; i < Size(); i++) {
      const Point2D next = Next(current).value();
      sum += (next.x - current.x) * (next.y + current.y);
      current = next;
    }
    return sum > 0;
  }

  void NormalizeDirection() {
    if (!IsClockwise())
      ReverseDirection();
  }

  std::map<Point2D, Point2D> prev_;
  std::map<Point2D, Point2D> next_;
  std::set<Point2D> points_;
};

std::vector<Point2D> AsVector(const Polygon2D& polygon) {
  if (polygon.Size() == 0)
    return {};
  
  std::vector<Point2D> result;
  geom::Point2D current = polygon.GetAnyPoint().value();
  for (size_t i = 0; i < polygon.Size();
       i++, current = polygon.Next(current).value()) {
    result.push_back(current);
  }
  return result;
}

}  // geom

#endif  // IMPL_POLYGON2D_HPP
