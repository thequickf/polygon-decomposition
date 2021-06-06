#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <impl/geom_utils.hpp>
#include <impl/polygon2d.hpp>

#include <iostream>

inline void PrintL() {
  std::cerr << std::endl;
}

inline void PrintPoint(const geom::Point2D& point) {
  std::cerr << "(" << point.x << ", " << point.y << ")";
}

inline void PrintPointL(const geom::Point2D& point) {
  PrintPoint(point);
  std::cerr << std::endl;
}

inline void PrintSegment2D(const geom::Segment2D& segment) {
  std::cerr << "(";
  PrintPoint(segment.a);
  std::cerr << ", ";
  PrintPoint(segment.b);
  std::cerr << ")";
}

inline void PrintSegment2DL(const geom::Segment2D& segment) {
  PrintSegment2D(segment);
  PrintL();
}

inline void PrintPolygon(const geom::Polygon2D& polygon) {
  if (polygon.Size() == 0)
    return;
  geom::Point2D current = polygon.GetAnyPoint().value();
  for (size_t i = 0; i < polygon.Size(); i++,
       current = polygon.Next(current).value()) {
    PrintPointL(current);
  }
  std::cerr << std::endl;

  return;
}

inline void PrintPolygonTypes(const geom::Polygon2D& polygon) {
  if (polygon.Size() < 3)
    return;
  geom::Point2D current = polygon.GetAnyPoint().value();
  for (size_t i = 0; i < polygon.Size();
       i++, current = polygon.Next(current).value()) {
    geom::Polygon2D::PointType type = polygon.GetPointType(current).value();
    switch (type) {
    case geom::Polygon2D::PointType::START:
      std::cerr << "Start   ";
      break;
    case geom::Polygon2D::PointType::END:
      std::cerr << "End     ";
      break;
    case geom::Polygon2D::PointType::SPLIT:
      std::cerr << "Split   ";
      break;
    case geom::Polygon2D::PointType::MERGE:
      std::cerr << "Merge   ";
      break;
    
    default:
      std::cerr << "Regular ";
      break;
    }
    PrintPointL(current);
  }
  std::cerr << std::endl;

  return;
}

#endif  // DEBUG_UTILS_HPP
