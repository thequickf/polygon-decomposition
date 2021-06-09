#ifndef TRIANGULATE_MONOTONE_H
#define TRIANGULATE_MONOTONE_H

#include <polygon2d.h>

#include <list>

namespace geom {

std::list<Polygon2D> TriangulateYMonotone(const Polygon2D& polygon);

}  // geom

#endif  // TRIANGULATE_MONOTONE_H
