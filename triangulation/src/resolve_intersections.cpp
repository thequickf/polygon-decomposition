#include <resolve_intersections.h>

#include <dcel_polygon2d.h>
#include <segments_on_y_sweep_line.h>

#include <cassert>
#include <memory>
#include <optional>
#include <set>

namespace geom {

namespace {

struct Event {
  enum EventType {
    BEGIN = 0,
    INTERSECTION = 1,
    END = 2
  };

  const Point2D point;
  const EventType type;

 protected:
  Event(const Point2D& point, const EventType type) :
      point(point), type(type) {}
};

struct BeginEvent : public Event {
  const Segment2D segment;
  BeginEvent(const Segment2D& segment) :
      Event(segment.a, BEGIN), segment(segment) {}
};

struct EndEvent : public Event {
  const Segment2D segment;
  EndEvent(const Segment2D& segment) :
      Event(segment.b, END), segment(segment) {}
};

struct IntersectionEvent : public Event {
  const Point2D a_end;
  const Point2D b_end;
  IntersectionEvent(const Point2D& point,
                    const Point2D& a_end,
                    const Point2D& b_end) :
      Event(point, INTERSECTION), a_end(a_end), b_end(b_end) {}
};

bool YFirstSegmentLess(const Segment2D& lhs, const Segment2D& rhs) {
  if (DoubleEqual(lhs.a, rhs.a))
    return YFirstPoint2DComparator()(lhs.b, rhs.b);
  return YFirstPoint2DComparator()(lhs.a, rhs.a);
}

struct EventComparator {
  bool operator()(const std::unique_ptr<const Event>& lhe,
                  const std::unique_ptr<const Event>& rhe) const {
    if (DoubleEqual(lhe->point, rhe->point)) {
      if (lhe->type == rhe->type) {
        switch (lhe->type) {
          case Event::BEGIN: {
            auto begin_lhe = static_cast<const BeginEvent*>(lhe.get());
            auto begin_rhe = static_cast<const BeginEvent*>(rhe.get());
            return YFirstSegmentLess(begin_lhe->segment, begin_rhe->segment);
          }
          case Event::END: {
            auto end_lhe = static_cast<const EndEvent*>(lhe.get());
            auto end_rhe = static_cast<const EndEvent*>(rhe.get());
            return YFirstSegmentLess(end_lhe->segment, end_rhe->segment);
          }
          case Event::INTERSECTION: {
            auto int_lhe = static_cast<const IntersectionEvent*>(lhe.get());
            auto int_rhe = static_cast<const IntersectionEvent*>(rhe.get());
            if (DoubleEqual(int_lhe->a_end, int_rhe->a_end))
              return YFirstPoint2DComparator()(int_lhe->b_end, int_rhe->b_end);
            else
              return YFirstPoint2DComparator()(int_lhe->a_end, int_rhe->a_end);
          }
        }
      }
      return lhe->type > rhe->type;
    }
    return YFirstPoint2DComparator()(lhe->point, rhe->point);
  }
};

class EventManager {
 public:
  const Event* Top() const {
    assert(!events_.empty());
    return events_.begin()->get();
  }

  bool Empty() const {
    return events_.empty();
  }

  void Pop() {
    events_.erase(events_.begin());
  }

  void AddSegment(const Segment2D& segment) {
    AddBegin(segment);
    AddEnd(segment);
  }

  void RemoveSegment(const Segment2D& segment) {
    RemoveBegin(segment);
    RemoveEnd(segment);
  }

  void AddBegin(const Segment2D& segment) {
    events_.insert(std::make_unique<const BeginEvent>(segment));
  }

  void AddEnd(const Segment2D& segment) {
    events_.insert(std::make_unique<const EndEvent>(segment));
  }

  void AddIntersection(
      const Point2D& point, const Point2D& a_end, const Point2D& b_end) {
    events_.insert(std::make_unique<const IntersectionEvent>(
        point, a_end, b_end));
  }

  void RemoveBegin(const Segment2D& segment) {
    events_.erase(std::make_unique<const BeginEvent>(segment));
  }

  void RemoveEnd(const Segment2D& segment) {
    events_.erase(std::make_unique<const EndEvent>(segment));
  }

 private:
  std::set<std::unique_ptr<const Event>, EventComparator> events_;
};

}  // namespace

std::list<Polygon2D> ResolveIntersections(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);
  EventManager events;
  SegmentsOnYSweepLine segments;

  const Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    Point2D a = current->point, b = current->next->point;
    if (!YFirstPoint2DComparator()(a, b))
      std::swap(a, b);
    events.AddSegment({a, b});
  }

  auto ResolveIntersection = [&](const Segment2D& segment_a,
                                 const Segment2D& segment_b) {
    std::optional<Point2D> int_point_opt =
        IntersectionPoint(segment_a, segment_b);
    if (!int_point_opt)
      return std::optional<Point2D>();

    if (IsIntersectionOnVertex(segment_a, segment_b))
      return std::optional<Point2D>();

    dcel_polygon.ResolveIntersection(segment_a, segment_b);

    events.RemoveSegment(segment_a);
    events.RemoveSegment(segment_b);

    const Point2D int_point = int_point_opt.value();
    const Segment2D a1int = {segment_a.a, int_point};
    const Segment2D b1int = {segment_b.a, int_point};

    events.AddEnd(a1int);
    events.AddEnd(b1int);

    events.AddIntersection(int_point, segment_a.b, segment_b.b);

    segments.Remove(segment_a);
    segments.Remove(segment_b);

    segments.Add(a1int);
    segments.Add(b1int);

    return std::optional<Point2D>(int_point);
  };

  while (!events.Empty()) {
    const Event* event = events.Top();
    SegmentsOnYSweepLine::SetY(event->point.y);

    switch (event->type) {
      case Event::BEGIN: {
        const BeginEvent begin_event = *(static_cast<const BeginEvent*>(event));
        events.Pop();

        std::optional<Segment2D> left = segments.FirstLeft(begin_event.segment);
        std::optional<Segment2D> right =
            segments.FirstRight(begin_event.segment);

        std::optional<Point2D> int_point;
        if (left)
          int_point = ResolveIntersection(begin_event.segment, *left);
        if (right && !int_point)
          int_point = ResolveIntersection(begin_event.segment, *right);

        if (int_point)
          events.AddBegin({begin_event.point, int_point.value()});
        else
          segments.Add(begin_event.segment);

        break;
      }
      case Event::END: {
        const EndEvent end_event = *(static_cast<const EndEvent*>(event));
        events.Pop();

        segments.Remove(end_event.segment);

        std::optional<Segment2D> left = segments.FirstLeft(end_event.segment);
        std::optional<Segment2D> right = segments.FirstRight(end_event.segment);
        if (left && right)
          ResolveIntersection(*left, *right);

        break;
      }
      case Event::INTERSECTION: {
        const IntersectionEvent int_event =
            *(static_cast<const IntersectionEvent*>(event));
        events.Pop();

        events.AddSegment({int_event.point, int_event.a_end});
        events.AddSegment({int_event.point, int_event.b_end});

        break;
      }
    }
  }

  return dcel_polygon.GetPolygons();
}

}  // geom
