#include <resolve_intersections.h>

#include <dcel_polygon2d.h>

#include <debug_utils.h>

#include <cassert>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>

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

/*
struct IntersectingSegments {
  Segment2D segment_a;
  Segment2D segment_b;

  IntersectingSegments() : segment_a({}, {}), segment_b({}, {}) {}
  IntersectingSegments(const Segment2D& segment_a, const Segment2D& segment_b) :
      segment_a(segment_a), segment_b(segment_b) {}
};

bool operator<(const Event& lhe, const Event& rhe) {
  if (DoubleEqual(lhe.point, rhe.point)) {
    if (lhe.type == rhe.type)
      return YFirstSegmentLess(lhe.segment, rhe.segment);
    return lhe.type > rhe.type;
  }
  return YFirstPoint2DComparator()(lhe.point, rhe.point);
}
//*/

}  // namespace

//*
void PrintEvent(const Event* event) {
  if (event->type == Event::BEGIN) {
    auto begin_event = static_cast<const BeginEvent*>(event);
    std::cerr << "Begin type, ";
    std::cerr << "Point: ";
    PrintPoint(begin_event->point);
    std::cerr << ", Segment: ";
    PrintSegment2D(begin_event->segment);
  } else if (event->type == Event::END) {
    auto end_event = static_cast<const EndEvent*>(event);
    std::cerr << "End type, ";
    std::cerr << "Point: ";
    PrintPoint(end_event->point);
    std::cerr << ", Segment: ";
    PrintSegment2D(end_event->segment);
  } else {
    auto int_event = static_cast<const IntersectionEvent*>(event);
    std::cerr << "Intersection type, ";
    std::cerr << "Point: ";
    PrintPoint(int_event->point);
    std::cerr << ", Segment A end: ";
    PrintPoint(int_event->a_end);
    std::cerr << ", Segment B end: ";
    PrintPoint(int_event->b_end);
  }
}

void PrintEventL(const Event* event) {
  PrintEvent(event);
  PrintL();
}

void PrintEvents(const std::set<std::unique_ptr<const Event>, EventComparator>& events) {
  for (const auto& event : events) {
    PrintEventL(event.get());
  }
  PrintL();
}
//*/

std::list<Polygon2D> ResolveIntersections(const Polygon2D& polygon) {
  if (polygon.Size() < 4)
    return {polygon};

  DcelPolygon2D dcel_polygon(polygon);

  std::set<std::unique_ptr<const Event>, EventComparator> events;

  auto AddSegmentToEvents = [&](const Segment2D& segment) {
    if (!events.insert(std::make_unique<const BeginEvent>(segment)).second) {
      std::cerr << "+++++++++++++++ not added begin event" << std::endl;
      std::cerr << "segment to add: ";
      PrintSegment2DL(segment);
      PrintEvents(events);
    }
    if (!events.insert(std::make_unique<const EndEvent>(segment)).second) {
      std::cerr << "+++++++++++++++ not added end event" << std::endl;
    }
  };

  auto RemoveSegmentFromEvents = [&](const Segment2D& segment) {
    // std::cerr << events.size() << std::endl;
    events.erase(std::make_unique<const BeginEvent>(segment));
    // std::cerr << events.size() << std::endl;
    events.erase(std::make_unique<const EndEvent>(segment));
    // std::cerr << events.size() << std::endl;
    // PrintL();
  };

  auto AddIntesectionToEvents = [&](const Point2D& int_point,
                                    const Point2D& a_end,
                                    const Point2D& b_end) {
    events.insert(std::make_unique<const IntersectionEvent>(
        int_point, a_end, b_end));
  };

  const Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    Point2D a = current->point, b = current->next->point;
    if (!YFirstPoint2DComparator()(a, b))
      std::swap(a, b);
    AddSegmentToEvents({a, b});
  }

  std::set<Segment2D, SegmentOnSweepLineComparator> segments_on_sweep_line;

  auto ResolveIntersection = [&](const Segment2D& segment_a,
                                 const Segment2D& segment_b) {
    std::optional<Point2D> int_point_opt =
        IntersectionPoint(segment_a, segment_b);
    if (!int_point_opt)
      return std::optional<Point2D>();

    if (IsIntersectionOnVertex(segment_a, segment_b))
      return std::optional<Point2D>();

    dcel_polygon.ResolveIntersection(segment_a, segment_b);

    RemoveSegmentFromEvents(segment_a);
    RemoveSegmentFromEvents(segment_b);

    const Point2D int_point = int_point_opt.value();
    const Segment2D a1int = {segment_a.a, int_point};
    const Segment2D b1int = {segment_b.a, int_point};

    /*
    std::cerr << "Intersection a:";
    PrintSegment2D(segment_a);
    std::cerr << ", b:";
    PrintSegment2D(segment_b);
    std::cerr << ", int:";
    PrintPointL(int_point);
    //*/

    if (!events.insert(std::make_unique<const EndEvent>(a1int)).second) {
      std::cerr << "+++++++++++++++ not added end event" << std::endl;
    }
    if (!events.insert(std::make_unique<const EndEvent>(b1int)).second) {
      std::cerr << "+++++++++++++++ not added end event" << std::endl;
    }
    // events.insert(std::make_unique<const EndEvent>(a1int));
    // events.insert(std::make_unique<const EndEvent>(b1int));

    AddIntesectionToEvents(int_point, segment_a.b, segment_b.b);

    // std::cerr << "before remove:"
    segments_on_sweep_line.erase(segment_a);
    // std::cerr << "Segment to remove: ";
    // PrintSegment2DL(segment_a);
    segments_on_sweep_line.erase(segment_b);
    // std::cerr << "Segment to remove: ";
    // PrintSegment2DL(segment_b);
    segments_on_sweep_line.insert(a1int);
    segments_on_sweep_line.insert(b1int);

    return std::optional<Point2D>(int_point);
  };

  auto FirstMoreByXOnSweepLine = [&](const Segment2D& segment) {
    auto right = segments_on_sweep_line.upper_bound(segment);
    for (; right != segments_on_sweep_line.end(); right++) {
      if (!DoubleEqual(SegmentOnSweepLineComparator::AnyXAtSweepLine(segment),
                       SegmentOnSweepLineComparator::AnyXAtSweepLine(*right)))
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
    auto left = segments_on_sweep_line.upper_bound(segment);
    left = PrevOnSweepLine(left);
    while (left != segments_on_sweep_line.end()) {
      if (!DoubleEqual(SegmentOnSweepLineComparator::AnyXAtSweepLine(segment),
                       SegmentOnSweepLineComparator::AnyXAtSweepLine(*left)))
        break;
      left = PrevOnSweepLine(left);
    }
    return left;
  };

  while (!events.empty()) {
    auto current_it = events.begin();
    const Event* event = current_it->get();
    // PrintEventL(event);
    SegmentOnSweepLineComparator::sweep_line_y = event->point.y;

    /*
    auto PrintSegmentsOnSweepLine = [&]() {
      std::cerr << "Segments on sweep line: " << std::endl;
      for (const Segment2D& segment : segments_on_sweep_line) {
        std::cerr << "{";
        PrintSegment2D(segment);
        std::cerr << ", x: ";
        std::cerr << SegmentOnSweepLineComparator::AnyXAtSweepLine(segment);
        std::cerr << "}, ";
      }
      PrintL();
      std::cerr << "Y: " << SegmentOnSweepLineComparator::sweep_line_y << std::endl;
    };

    // PrintSegmentsOnSweepLine();


    for (auto seg_it = segments_on_sweep_line.begin(); seg_it != segments_on_sweep_line.end(); seg_it++) {
      auto next_it = seg_it;
      next_it++;
      if (next_it != segments_on_sweep_line.end()) {
        double current_x = SegmentOnSweepLineComparator::AnyXAtSweepLine(*seg_it),
               next_x = SegmentOnSweepLineComparator::AnyXAtSweepLine(*next_it);
        if (!DoubleEqual(current_x, next_x)) {
          if (current_x > next_x) {
            PrintSegmentsOnSweepLine();
            assert(false);
          }
        }
      }
    }

    auto RemoveSegmentOnSweepLine = [&](const Segment2D& segment) {
      segments_on_sweep_line.erase(segment);
    };
    //*/

    switch (event->type) {
      case Event::BEGIN: {
        const BeginEvent begin_event = *(static_cast<const BeginEvent*>(event));
        events.erase(current_it);

        auto left = FirstLessByXOnSweepLine(begin_event.segment);
        auto right = FirstMoreByXOnSweepLine(begin_event.segment);
        
        bool left_preset = left != segments_on_sweep_line.end();
        bool right_preset = right != segments_on_sweep_line.end();
        std::optional<Point2D> int_point;
        if (left_preset) {
          // std::cerr << "left: ";
          // PrintSegment2DL(*left);
          int_point = ResolveIntersection(begin_event.segment, *left);
        }
        if (right_preset && !int_point) {
          // std::cerr << "right: ";
          // PrintSegment2DL(*right);
          int_point = ResolveIntersection(begin_event.segment, *right);
        }

        // begin_event = static_cast<const BeginEvent*>(it->get());
        if (int_point) {
          // std::cerr << "Segment added: " << std::endl;
          // PrintSegment2DL(begin_event->segment);
          events.insert(std::make_unique<const BeginEvent>(
              Segment2D(begin_event.point, int_point.value())));
        } else {
          segments_on_sweep_line.insert(begin_event.segment);
        }

        break;
      }
      case Event::END: {
        const EndEvent end_event = *(static_cast<const EndEvent*>(event));
        events.erase(current_it);

        segments_on_sweep_line.erase(end_event.segment);

        auto left = FirstLessByXOnSweepLine(end_event.segment);
        auto right = FirstMoreByXOnSweepLine(end_event.segment);
        bool left_preset = left != segments_on_sweep_line.end();
        bool right_preset = right != segments_on_sweep_line.end();
        if (left_preset && right_preset)
          ResolveIntersection(*left, *right);
        // segments_on_sweep_line.erase(end_event->segment);
        break;
      }
      case Event::INTERSECTION: {
        const IntersectionEvent int_event =
            *(static_cast<const IntersectionEvent*>(event));
        events.erase(current_it);

        AddSegmentToEvents({int_event.point, int_event.a_end});
        AddSegmentToEvents({int_event.point, int_event.b_end});
        // const Point2D a_end = int_event->a_end;
        // const Segment2D inta2 = {int_event->point, int_event->a_end};
        // const Point2D a_begin = top_to_bottom[inta2];
        // const Segment2D a_segment
        // const Point2D a_end = int_event->segment_a.b;
        // const Point2D a_begin = top_to_bottom[int_event->segment_a];
        // const Segment2D a1int = {segment_a.a, int_point};
        // const Segment2D b1int = {segment_b.a, int_point};
        // const Segment2D inta2 = {int_point, segment_a.b};
        // const Segment2D intb2 = {int_point, segment_b.b};
        break;
      }
    }
  }

  return dcel_polygon.GetPolygons();
}

}  // geom
