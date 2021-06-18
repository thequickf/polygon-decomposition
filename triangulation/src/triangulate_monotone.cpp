#include <triangulate_monotone.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>

#include <algorithm>
#include <stack>
#include <vector>

namespace geom {

namespace {

bool IsAdjacent(const Polygon2D::Vertex* a, const Polygon2D::Vertex* b) {
  return a->prev == b || a->next == b;
}

bool IsValidDiagonal(const Polygon2D::Vertex* current,
                     const Polygon2D::Vertex* last,
                     const Polygon2D::Vertex* to_process) {
  Vector2D v = {current->point, last->point};
  Vector2D u = {current->point, to_process->point};
  bool right = MoreThenPiAngle2D(u, v);
  return (current->type == Polygon2D::RIGHT_REGULAR) == right;
}

}  // namespace

// Moving with y sweep line insering edges as long as we can
std::list<Polygon2D> TriangulateYMonotone(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);
  std::vector<const Polygon2D::Vertex*> vertices = AsVertexVector(polygon);
  std::sort(vertices.rbegin(), vertices.rend(), YFirstVertexComparator());
  std::stack<const Polygon2D::Vertex*> to_process_stk;
  to_process_stk.push(vertices[0]);
  to_process_stk.push(vertices[1]);
  size_t i = 2;
  for (; i < vertices.size() - 1; i++) {
    if (IsAdjacent(vertices[i], to_process_stk.top())) {
      const Polygon2D::Vertex* last = to_process_stk.top();
      to_process_stk.pop();
      while (to_process_stk.size() > 0 &&
          IsValidDiagonal(vertices[i], last, to_process_stk.top())) {
        last = to_process_stk.top();
        to_process_stk.pop();
        dcel_polygon.InsertEdge({vertices[i]->point, last->point});
      }
      to_process_stk.push(last);
      to_process_stk.push(vertices[i]);
    } else {
      while (to_process_stk.size() > 0) {
        if (to_process_stk.size() != 1) {
          dcel_polygon.InsertEdge(
              {vertices[i]->point, to_process_stk.top()->point});
        }
        to_process_stk.pop();
      }
      to_process_stk.push(vertices[i - 1]);
      to_process_stk.push(vertices[i]);
    }
  }
  to_process_stk.pop();
  while (to_process_stk.size() > 0) {
    if (to_process_stk.size() != 1) {
      dcel_polygon.InsertEdge(
          {vertices[i]->point, to_process_stk.top()->point});
    }
    to_process_stk.pop();
  }
  return dcel_polygon.GetPolygons();
}

}  // geom
