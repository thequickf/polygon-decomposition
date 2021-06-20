#ifndef SEGMENTS_ON_Y_SWEEP_LINE_H
#define SEGMENTS_ON_Y_SWEEP_LINE_H

#include <geom_utils.h>

#include <optional>
#include <set>

namespace geom {

class SegmentsOnYSweepLine {
 public:
  void Add(const Segment2D& segment);
  void Remove(const Segment2D& segment);

  std::optional<Segment2D> FirstLeft(const Segment2D& segment) const;
  std::optional<Segment2D> FirstLeft(const Point2D& point) const;
  std::optional<Segment2D> FirstRight(const Segment2D& segment) const;

  static void SetY(double y);

 private:
  struct SegmentOnSweepLineComparator {
    bool operator()(const Segment2D& lhs, const Segment2D& rhs) const;
  };

  static double y;

  static double AnyXAtSweepLine(const Segment2D& segment);

  using iterator = std::set<Segment2D, SegmentOnSweepLineComparator>::iterator;
  iterator PrevIt(iterator it) const;

  std::set<Segment2D, SegmentOnSweepLineComparator> segments_on_sweep_line_;
};

}  // geom

#endif  // SEGMENTS_ON_Y_SWEEP_LINE_H
