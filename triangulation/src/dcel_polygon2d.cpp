#include <dcel_polygon2d.h>

#include <cmath>
#include <unordered_map>
#include <utility>

namespace geom {

namespace {

bool IsPointLeftToSegment(const Segment2D& segment, const Point2D& point) {
  const Vector2D v = {segment.a, point};
  const Vector2D u = {segment.a, segment.b};
  return MoreThenPiAngle2D(v, u);
}

}  // namespace

DcelPolygon2D::HalfEdge::HalfEdge(const Vertex* origin, const Vector2D& v) :
    origin(origin), angle(std::atan2(v.y, v.x)) {}

std::tuple<const DcelPolygon2D::HalfEdge*, const DcelPolygon2D::HalfEdge*>
    DcelPolygon2D::Vertex::GetNeighbourHalfEdges(
    const HalfEdge* edge) const {
  auto right = edges.upper_bound(edge);
  if (right == edges.end())
    right = edges.begin();

  auto left = edges.upper_bound(edge);
  if (left == edges.begin())
    left = edges.end();
  left--;
  
  return std::make_tuple(*left, *right);
}

bool operator<(const DcelPolygon2D::Vertex& lhv,
               const DcelPolygon2D::Vertex& rhv) {
  return lhv.point < rhv.point;
}

bool operator==(const DcelPolygon2D::Face& lhf,
                const DcelPolygon2D::Face& rhf) {
  return lhf.edge == rhf.edge;
}

bool operator!=(const DcelPolygon2D::Face& lhf,
                const DcelPolygon2D::Face& rhf) {
  return !(lhf == rhf);
}

DcelPolygon2D::DcelPolygon2D(const Polygon2D& polygon2D) {
  std::unordered_map<const Polygon2D::Vertex*, const Vertex*> pnt_to_vertex;
  std::unordered_map<const Polygon2D::Vertex*, const HalfEdge*>
      pnt_to_edge_forward;

  const Polygon2D::Vertex* current = polygon2D.GetAnyVertex();
  for (size_t i = 0; i < polygon2D.Size(); i++) {
    const Polygon2D::Vertex* next = current->next;

    const Vertex* vertex = &*vertices_.insert(Vertex(current->point)).first;
    pnt_to_vertex[current] = vertex;

    half_edges_.push_back(HalfEdge(vertex, {current->point, next->point}));
    const HalfEdge* edge = &half_edges_.back();
    vertex->edges.insert(edge);
    pnt_to_edge_forward[current] = edge;

    current = next;
  }

  for (size_t i = 0; i < polygon2D.Size(); i++) {
    const Polygon2D::Vertex* prev = current->prev;
    const Polygon2D::Vertex* next = current->next;

    const HalfEdge* edge = pnt_to_edge_forward[current];
    edge->prev = pnt_to_edge_forward[prev];
    edge->next = pnt_to_edge_forward[next];

    current = next;
  }

  faces_.push_back(Face(pnt_to_edge_forward[current]));

  std::unordered_map<const Polygon2D::Vertex*, const HalfEdge*>
      pnt_to_edge_back;
  for (size_t i = 0; i < polygon2D.Size(); i++) {
    const Polygon2D::Vertex* next = current->next;

    const Vertex* vertex = pnt_to_vertex[next];

    half_edges_.push_back(HalfEdge(vertex, {next->point, current->point}));
    const HalfEdge* edge = &half_edges_.back();
    vertex->edges.insert(edge);

    pnt_to_edge_forward[current]->twin = edge;
    edge->twin = pnt_to_edge_forward[current];

    pnt_to_edge_back[next] = edge;

    current = next;
  }

  for (size_t i = 0; i < polygon2D.Size(); i++) {
    const Polygon2D::Vertex* prev = current->prev;
    const Polygon2D::Vertex* next = current->next;

    const HalfEdge* edge = pnt_to_edge_back[current];
    edge->prev = pnt_to_edge_back[next];
    edge->next = pnt_to_edge_back[prev];

    current = next;
  }

  Face external_face(pnt_to_edge_forward[current]->twin);
  faces_.push_back(external_face);
}

void DcelPolygon2D::InsertEdge(const Segment2D& edge) {
  auto u_it = vertices_.find(Vertex(edge.a));
  auto v_it = vertices_.find(Vertex(edge.b));
  if (u_it == vertices_.end() || v_it == vertices_.end())
    return;

  const Vertex* u = &*u_it;
  const Vertex* v = &*v_it;

  half_edges_.push_back(HalfEdge(u, {edge.a, edge.b}));
  const HalfEdge* uv_edge = &half_edges_.back();
  half_edges_.push_back(HalfEdge(v, {edge.b, edge.a}));
  const HalfEdge* vu_edge = &half_edges_.back();

  uv_edge->twin = vu_edge;
  vu_edge->twin = uv_edge;

  const HalfEdge* u_left;
  const HalfEdge* u_right;
  const HalfEdge* v_left;
  const HalfEdge* v_right;
  std::tie(u_left, u_right) = u->GetNeighbourHalfEdges(uv_edge);
  std::tie(v_left, v_right) = v->GetNeighbourHalfEdges(vu_edge);
  
  u_left->twin->next = uv_edge;
  uv_edge->prev = u_left->twin;
  uv_edge->next = v_right;
  v_right->prev = uv_edge;

  v_left->twin->next = vu_edge;
  vu_edge->prev = v_left->twin;
  vu_edge->next = u_right;
  u_right->prev = vu_edge;

  u->edges.insert(uv_edge);
  v->edges.insert(vu_edge);

  faces_.push_back(Face(vu_edge));
  faces_.push_back(Face(uv_edge));
}

// a1     b2
//   \   /  
//    \ /   
//    int   
//    / \    
//   /   \  
// b1     a2
void DcelPolygon2D::ResolveIntersection(const Segment2D& a,
                                        const Segment2D& b) {
  const Point2D a1_pnt = a.a;
  const Point2D a2_pnt = a.b;
  Point2D b1_pnt = b.a;
  Point2D b2_pnt = b.b;
  if (IsPointLeftToSegment(a, b.a))
    std::swap(b1_pnt, b2_pnt);

  const std::optional<Point2D> intersection_opt = IntersectionPoint(a, b);
  const auto a1_it = vertices_.find(Vertex(a1_pnt));
  const auto a2_it = vertices_.find(Vertex(a2_pnt));
  const auto b1_it = vertices_.find(Vertex(b1_pnt));
  const auto b2_it = vertices_.find(Vertex(b2_pnt));
  const auto vertices_end = vertices_.end();
  if (a1_it == vertices_end || a2_it == vertices_end ||
      b1_it == vertices_end || b2_it == vertices_end || !intersection_opt)
    return;
  const Vertex* a1 = &*a1_it;
  const Vertex* a2 = &*a2_it;
  const Vertex* b1 = &*b1_it;
  const Vertex* b2 = &*b2_it;
  const std::optional<const HalfEdge*> a1a2he_opt = GetHalfEdge(a1, a2);
  const std::optional<const HalfEdge*> b1b2he_opt = GetHalfEdge(b1, b2);
  if (!a1a2he_opt || !b1b2he_opt)
    return;
  const HalfEdge* a1a2he = a1a2he_opt.value();
  const HalfEdge* a2a1he = a1a2he->twin;
  const HalfEdge* b1b2he = b1b2he_opt.value();
  const HalfEdge* b2b1he = b1b2he->twin;

  const Point2D intersection_point = intersection_opt.value();
  auto existing_intersection_vertex_it =
      vertices_.find(Vertex(intersection_point));
  const Vertex* intersection;
  if (existing_intersection_vertex_it != vertices_.end())
    intersection = &*existing_intersection_vertex_it;
  else
    intersection = &*vertices_.insert(Vertex(intersection_point)).first;

  half_edges_.push_back(HalfEdge(intersection, {intersection->point, a1_pnt}));
  const HalfEdge* inta1he = &half_edges_.back();
  half_edges_.push_back(HalfEdge(intersection, {intersection->point, a2_pnt}));
  const HalfEdge* inta2he = &half_edges_.back();
  half_edges_.push_back(HalfEdge(intersection, {intersection->point, b1_pnt}));
  const HalfEdge* intb1he = &half_edges_.back();
  half_edges_.push_back(HalfEdge(intersection, {intersection->point, b2_pnt}));
  const HalfEdge* intb2he = &half_edges_.back();

  intersection->edges.insert(inta1he);
  intersection->edges.insert(inta2he);
  intersection->edges.insert(intb1he);
  intersection->edges.insert(intb2he);

  inta1he->twin = a1a2he;
  a1a2he->twin = inta1he;

  inta2he->twin = a2a1he;
  a2a1he->twin = inta2he;

  intb1he->twin = b1b2he;
  b1b2he->twin = intb1he;

  intb2he->twin = b2b1he;
  b2b1he->twin = intb2he;

  inta1he->next = a2a1he->next;
  inta2he->next = a1a2he->next;
  intb1he->next = b2b1he->next;
  intb2he->next = b1b2he->next;

  inta1he->prev = b2b1he;
  b2b1he->next = inta1he;

  inta2he->prev = b1b2he;
  b1b2he->next = inta2he;

  intb1he->prev = a1a2he;
  a1a2he->next = intb1he;

  intb2he->prev = a2a1he;
  a2a1he->next = intb2he;

  inta1he->next->prev = inta1he;
  inta2he->next->prev = inta2he;
  intb1he->next->prev = intb1he;
  intb2he->next->prev = intb2he;

  faces_.push_back(Face(inta1he));
  faces_.push_back(Face(inta2he));
  faces_.push_back(Face(intb1he));
  faces_.push_back(Face(intb2he));
}

std::list<Polygon2D> DcelPolygon2D::GetPolygons() const {
  struct PolygonWithArea {
    long double area;
    std::vector<Point2D> polygon_v;

    PolygonWithArea(long double area, std::vector<Point2D>&& polygon_v) :
        area(area), polygon_v(std::forward<std::vector<Point2D>>(polygon_v)) {}
  };

  std::set<const HalfEdge*> visited;
  std::list<PolygonWithArea> res_polygons;
  long double max_area = 0;
  for (const Face& face : faces_) {
    const HalfEdge* start_edge = face.edge;
    if (visited.count(start_edge))
      continue;
    std::vector<Point2D> polygon_vector;
    const HalfEdge* edge = face.edge;
    long double area = 0;
    do {
      const Point2D current_pnt = edge->origin->point;
      const Point2D next_pnt = edge->next->origin->point;
      polygon_vector.push_back(edge->origin->point);
      visited.insert(edge);

      area += (next_pnt.x - current_pnt.x) * (next_pnt.y + current_pnt.y);
      edge = edge->next;
    } while (edge != start_edge);

    area = std::fabs(area);
    max_area = std::max(max_area, area);
    res_polygons.push_back({area, std::move(polygon_vector)});
  }

  std::list<Polygon2D> res;
  bool external_exclided = false;
  for (const PolygonWithArea& res_polygon : res_polygons)
    if (res_polygon.area != max_area || external_exclided)
      res.push_back(Polygon2D(res_polygon.polygon_v));
    else
      external_exclided = true;

  return res;
}

std::optional<const DcelPolygon2D::HalfEdge*> DcelPolygon2D::GetHalfEdge(
    const Vertex* a, const Vertex* b) const {
  const HalfEdge half_edge_for_search = {a, {a->point, b->point}};
  const auto half_edge_it = a->edges.lower_bound(&half_edge_for_search);
  if (half_edge_it == a->edges.end())
    return {};
  const HalfEdge* half_edge = *half_edge_it;
  if (half_edge->next->origin == b)
    return half_edge;
  return {};
}

}  // geom
