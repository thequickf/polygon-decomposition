#include <segments_on_y_sweep_line.h>

#include <cassert>

namespace geom {

double SegmentsOnYSweepLine::y = 0;

bool SegmentsOnYSweepLine::SegmentOnSweepLineComparator::operator()(
    const Segment2D& lhs, const Segment2D& rhs) const {
  if (DoubleEqual(lhs, rhs))
    return false;
  const double lhx = AnyXAtSweepLine(lhs), rhx = AnyXAtSweepLine(rhs);
  if (DoubleEqual(lhx, rhx)) {
    const bool left = MoreThenPiAngle2D({lhs.a, lhs.b}, {rhs.a, rhs.b});
    if (DoubleEqual(lhs.a, rhs.a))
      return left;
    if (DoubleEqual(lhs.b, rhs.b))
      return !left;
    assert(false);
    return false;
  }
  return lhx < rhx;
}

void SegmentsOnYSweepLine::Add(const Segment2D& segment) {
  segments_on_sweep_line_.insert(segment);
}

void SegmentsOnYSweepLine::Remove(const Segment2D& segment) {
  segments_on_sweep_line_.erase(segment);
}

std::optional<Segment2D> SegmentsOnYSweepLine::FirstLeft(
    const Segment2D& segment) const {
  auto left = segments_on_sweep_line_.upper_bound(segment);
  left = PrevIt(left);
  while (left != segments_on_sweep_line_.end()) {
    if (!DoubleEqual(AnyXAtSweepLine(segment), AnyXAtSweepLine(*left)))
      break;
    left = PrevIt(left);
  }
  if (left != segments_on_sweep_line_.end())
    return *left;
  return {};
}

std::optional<Segment2D> SegmentsOnYSweepLine::FirstLeft(
    const Point2D& point) const {
  return FirstLeft({point, point});
}

std::optional<Segment2D> SegmentsOnYSweepLine::FirstRight(
    const Segment2D& segment) const {
  auto right = segments_on_sweep_line_.upper_bound(segment);
  for (; right != segments_on_sweep_line_.end(); right++) {
    if (!DoubleEqual(AnyXAtSweepLine(segment), AnyXAtSweepLine(*right)))
      break;
  }
  if (right != segments_on_sweep_line_.end())
    return *right;
  return {};
}

void SegmentsOnYSweepLine::SetY(double y) {
  SegmentsOnYSweepLine::y = y;
}

SegmentsOnYSweepLine::iterator SegmentsOnYSweepLine::PrevIt(
    SegmentsOnYSweepLine::iterator it) const {
  if (it == segments_on_sweep_line_.begin())
    return segments_on_sweep_line_.end();
  if (segments_on_sweep_line_.size() == 0)
    return segments_on_sweep_line_.end();
  return std::prev(it);
}

double SegmentsOnYSweepLine::AnyXAtSweepLine(
    const Segment2D& segment) {
  const Vector2D v = {segment.a, segment.b};
  if (v.y == 0)
    return segment.a.x;
  double k = (y - segment.a.y) / (v.y);
  return segment.a.x + v.x * k;
}

}  // geom
