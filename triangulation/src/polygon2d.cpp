#include <polygon2d.h>

namespace geom {

Polygon2D::Polygon2D(const std::vector<Point2D>& points) {
  if (points.size() == 0)
    return;
  
  vertices_.push_back(Vertex(points[0]));
  Vertex* prev = &vertices_.back();
  for (size_t i = 1; i < points.size(); i++) {
    vertices_.push_back(Vertex(points[i]));
    Vertex* current = &vertices_.back();
    current->prev = prev;
    prev->next = current;
    prev = current;
  }
  Vertex* first = &vertices_.front();
  Vertex* last = &vertices_.back();
  first->prev = last;
  last->next = first;

  NormalizeDirection();
  SetVertexTypes();
}


size_t Polygon2D::Size() const {
  return vertices_.size();
}

const Polygon2D::Vertex* Polygon2D::GetAnyVertex() const {
  if (Size() > 0)
    return &vertices_.front();
  return nullptr;
}

Polygon2D::VertexType Polygon2D::GetVertexType(const Vertex* vertex) {
  const Point2D prev = vertex->prev->point;
  const Point2D current = vertex->point;
  const Point2D next = vertex->next->point;
  const bool start_kind = YFirstPoint2DComparator()(prev, current) &&
                          YFirstPoint2DComparator()(next, current);
  const bool end_kind = YFirstPoint2DComparator()(current, prev) &&
                        YFirstPoint2DComparator()(current, next);
  if (start_kind || end_kind) {
    if (MoreThenPiAngle2D({current, prev}, {current, next}))
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
    if (YFirstPoint2DComparator()(current, next))
      return LEFT_REGULAR;
    return RIGHT_REGULAR;
  }
}

void Polygon2D::SetVertexTypes() {
  if (Size() == 0)
    return;

  Vertex* current = &vertices_.front();
  for (size_t i = 0; i < Size(); i++, current = current->next)
    current->type = GetVertexType(current);
}

void Polygon2D::ReverseDirection() {
  if (Size() == 0)
    return;

  Vertex* current = &vertices_.front();
  for (size_t i = 0; i < Size(); i++, current = current->next)
    std::swap(current->prev, current->next);
}

bool Polygon2D::IsClockwise() const {
  if (Size() == 0)
    return false;

  const Vertex* current = GetAnyVertex();
  long double area = 0;
  for (size_t i = 0; i < Size(); i++) {
    const Vertex* next = current->next;
    area += (next->point.x - current->point.x) *
            (next->point.y + current->point.y);
    current = next;
  }
  return area > 0;
}

void Polygon2D::NormalizeDirection() {
  if (!IsClockwise())
    ReverseDirection();
}

bool YFirstVertexComparator::operator()(
    const Polygon2D::Vertex* const& lhv,
    const Polygon2D::Vertex* const& rhv) const {
  if (lhv->point != rhv->point)
    return YFirstPoint2DComparator()(lhv->point, rhv->point);
  if (lhv->type != rhv->type)
    return lhv->type < rhv->type;
  if (lhv->prev != rhv->prev)
    return lhv->prev < rhv->prev;
  return lhv->next < rhv->next;
}

std::vector<Point2D> AsVector(const Polygon2D& polygon) {
  if (polygon.Size() == 0)
    return {};
  
  std::vector<Point2D> result;
  const Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next)
    result.push_back(current->point);
  return result;
}

std::vector<const Polygon2D::Vertex*> AsVertexVector(const Polygon2D& polygon) {
  if (polygon.Size() == 0)
    return {};
  
  std::vector<const Polygon2D::Vertex*> result;
  const Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next)
    result.push_back(current);
  return result;
}

}  // geom
