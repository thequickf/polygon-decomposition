#include <resolve_intersections.h>

#include <dcel_polygon2d.h>

#include <debug_utils.h>

#include <set>
#include <tuple>

namespace geom {

namespace {

struct Event {
  enum EventType {
    BEGIN = 0,
    END = 1
  };

  Point2D point;
  Segment2D segment;
  EventType type;

  Event(const Point2D& point, const Segment2D& segment, EventType type) :
      point(point), segment(segment), type(type) {}
};

bool YFirstSegmentLess(const Segment2D& lhs, const Segment2D& rhs) {
  if (DoubleEqual(lhs.a, rhs.a))
    return YFirstPoint2DComparator()(lhs.b, rhs.b);
  return YFirstPoint2DComparator()(lhs.a, rhs.a);
}

bool operator<(const Event& lhe, const Event& rhe) {
  if (DoubleEqual(lhe.point, rhe.point)) {
    if (lhe.type == rhe.type)
      return YFirstSegmentLess(lhe.segment, rhe.segment);
    return lhe.type > rhe.type;
  }
  return YFirstPoint2DComparator()(lhe.point, rhe.point);
}

}  // namespace

/*
void PrintEvent(const Event& event) {
  if (event.type == Event::END)
    std::cerr << "End ";
  else
    std::cerr << "Begin ";
  PrintPoint(event.point);
  std::cerr << " ";
  PrintSegment2D(event.segment);
}

void PrintEventL(const Event& event) {
  PrintEvent(event);
  PrintL();
}
//*/

bool operator==(const Segment2D& lhs, const Segment2D& rhs) {
  return DoubleEqual(lhs.a, rhs.a) && DoubleEqual(lhs.b, rhs.b);
}

std::list<Polygon2D> ResolveIntersections(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);

  std::set<Event> events;

  auto AddSegmentToEvents = [&](const Segment2D& segment) {
      auto res = events.insert({segment.a, segment, Event::BEGIN}).first;
      events.insert({segment.b, segment, Event::END});
      return res;
  };

  auto RemoveSegmentFromEvents = [&](const Segment2D& segment) {
      events.erase({segment.a, segment, Event::BEGIN});
      events.erase({segment.b, segment, Event::END});
  };

  /*
  size_t number_of_intersections = 0;
  std::vector<Segment2D> segments;
  const Polygon2D::Vertex* vertex = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, vertex = vertex->next) {
    Point2D a = vertex->point, b = vertex->next->point;
    segments.push_back({a, b});
  }
  for (size_t i = 0; i < segments.size(); i++) {
    for (size_t j = i + 1; j < segments.size(); j++) {
      std::optional<Point2D> int_opt = IntersectionPoint(segments[i], segments[j]);
      if (int_opt) {
        if (!IsIntersectionOnVertex(segments[i], segments[j])) {
          number_of_intersections++;
        }
      }
    }
  }
  std::cerr << "number_of_intersections: " << number_of_intersections << std::endl;
  //*/

  const Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    Point2D a = current->point, b = current->next->point;
    if (!YFirstPoint2DComparator()(a, b))
      std::swap(a, b);
    AddSegmentToEvents({a, b});
  }

  std::set<Segment2D, SegmentOnSweepLineComparator> segments_on_sweep_line;
  for (auto it = events.begin(); it != events.end(); it++) {
    SegmentOnSweepLineComparator::sweep_line_y = it->point.y;

    if (it->type == Event::BEGIN) {
      auto FirstMoreByXOnSweepLine = [&](const Segment2D& segment) {
        auto right = segments_on_sweep_line.upper_bound(it->segment);
        for (; right != segments_on_sweep_line.end(); right++) {
          if (!IsIntersectionOnVertex(segment, *right))
            break;
        }
        return right;
      };
      
      auto PrevOnSweepLine = [&](
          std::set<Segment2D, SegmentOnSweepLineComparator>::iterator jt) {
        if (jt == segments_on_sweep_line.begin())
          return segments_on_sweep_line.end();
        if (segments_on_sweep_line.size() == 0)
          return segments_on_sweep_line.end();
        return std::prev(jt);
      };

      auto FirstLessByXOnSweepLine = [&](const Segment2D& segment) {
        auto left = segments_on_sweep_line.upper_bound(it->segment);
        left = PrevOnSweepLine(left);
        while (left != segments_on_sweep_line.end()) {
          if (!IsIntersectionOnVertex(segment, *left))
            break;
          left = PrevOnSweepLine(left);
        }
        return left;
      };

      auto left = FirstLessByXOnSweepLine(it->segment);
      auto right = FirstMoreByXOnSweepLine(it->segment);
      
      auto ResolveIntersection = [&](
          std::set<Segment2D, SegmentOnSweepLineComparator>::iterator jt) {
        if (jt == segments_on_sweep_line.end())
          return;
        const Segment2D segment_a = it->segment;
        const Segment2D segment_b = *jt;
        std::optional<Point2D> int_point_opt =
            IntersectionPoint(segment_a, segment_b);
        if (!int_point_opt)
          return;
        
        /*
        std::cerr << "Intersection ";
        PrintSegment2D(segment_a);
        std::cerr << ", ";
        PrintSegment2D(segment_b);
        std::cerr << " ";
        PrintPoint(int_point_opt.value());
        PrintL();
        //*/

        dcel_polygon.ResolveIntersection(segment_a, segment_b);

        RemoveSegmentFromEvents(segment_a);
        RemoveSegmentFromEvents(segment_b);

        const Point2D int_point = int_point_opt.value();
        const Segment2D a1int = {segment_a.a, int_point};
        const Segment2D b1int = {segment_b.a, int_point};
        const Segment2D inta2 = {int_point, segment_a.b};
        const Segment2D intb2 = {int_point, segment_b.b};

        it = AddSegmentToEvents(a1int);
        AddSegmentToEvents(b1int);
        AddSegmentToEvents(inta2);
        AddSegmentToEvents(intb2);

        auto tmp_it = segments_on_sweep_line.find(segment_b);
        if (tmp_it != segments_on_sweep_line.end())
          segments_on_sweep_line.erase(tmp_it);

        segments_on_sweep_line.insert(b1int);
      };
      
      ResolveIntersection(left);
      ResolveIntersection(right);

      segments_on_sweep_line.insert(it->segment);
    } else if (it->type == Event::END) {
      auto tmp_it = segments_on_sweep_line.find(it->segment);
      if (tmp_it != segments_on_sweep_line.end())
        segments_on_sweep_line.erase(tmp_it);
      segments_on_sweep_line.erase(it->segment);
    }
  }

  return dcel_polygon.GetPolygons();
}

}  // geom
