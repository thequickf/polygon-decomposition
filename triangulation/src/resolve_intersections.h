#ifndef RESOLVE_INTERSECTIONS_H
#define RESOLVE_INTERSECTIONS_H

#include <geom_utils.h>
#include <polygon2d.h>

#include <list>

namespace geom {

std::list<Polygon2D> ResolveIntersections(const Polygon2D& polygon);

}  // geom

#endif  // RESOLVE_INTERSECTIONS_H
