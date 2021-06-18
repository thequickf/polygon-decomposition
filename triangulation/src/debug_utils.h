#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <geom_utils.h>
#include <polygon2d.h>
#include <dcel_polygon2d.h>

#include <iostream>

static void PrintL() {
  std::cerr << std::endl;
}

static void PrintPoint(const geom::Point2D& point) {
  std::cerr << "(" << point.x << ", " << point.y << ")";
}

static void PrintPointL(const geom::Point2D& point) {
  PrintPoint(point);
  std::cerr << std::endl;
}

static void PrintSegment2D(const geom::Segment2D& segment) {
  std::cerr << "(";
  PrintPoint(segment.a);
  std::cerr << ", ";
  PrintPoint(segment.b);
  std::cerr << ")";
}

static void PrintSegment2DL(const geom::Segment2D& segment) {
  PrintSegment2D(segment);
  PrintL();
}

static void PrintPolygon(const geom::Polygon2D& polygon) {
  if (polygon.Size() == 0)
    return;
  const geom::Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    PrintPointL(current->point);
  }
  std::cerr << std::endl;

  return;
}

static void PrintPolygonTypes(const geom::Polygon2D& polygon) {
  if (polygon.Size() < 3)
    return;
  const geom::Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    switch (current->type) {
    case geom::Polygon2D::START:
      std::cerr << "Start   ";
      break;
    case geom::Polygon2D::END:
      std::cerr << "End     ";
      break;
    case geom::Polygon2D::SPLIT:
      std::cerr << "Split   ";
      break;
    case geom::Polygon2D::MERGE:
      std::cerr << "Merge   ";
      break;
    case geom::Polygon2D::LEFT_REGULAR:
      std::cerr << "LeftRegular" << std::endl;
      break;
    case geom::Polygon2D::RIGHT_REGULAR:
      std::cerr << "RightRegular" << std::endl;
      break;
    }
    PrintPointL(current->point);
  }
  std::cerr << std::endl;

  return;
}

static void PrintHalfEdge(const geom::HalfEdge* edge) {
  const geom::HalfEdge* current = edge;
  PrintPoint(current->origin->point);
  std::cerr << " -> ";
  current = current->next;
  PrintPoint(current->origin->point);
}

static void PrintHalfEdgeL(const geom::HalfEdge* edge) {
  PrintHalfEdge(edge);
  PrintL();
}

static void Print3he(const geom::HalfEdge* edge) {
  const geom::HalfEdge* current = edge;
  PrintPoint(current->origin->point);
  current = current->next;
  std::cerr << " -> ";
  PrintPoint(current->origin->point);
  current = current->next;
  std::cerr << " -> ";
  PrintPoint(current->origin->point);
  PrintL();

  current = current->prev;
  current = current->twin;
  PrintPoint(current->origin->point);
  current = current->next;
  std::cerr << " -> ";
  PrintPoint(current->origin->point);
  current = current->next;
  std::cerr << " -> ";
  PrintPoint(current->origin->point);
  PrintL();
}

#endif  // DEBUG_UTILS_H
