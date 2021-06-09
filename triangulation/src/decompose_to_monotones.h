#ifndef DECOMPOSE_TO_MONOTONES_H
#define DECOMPOSE_TO_MONOTONES_H

#include <geom_utils.h>
#include <polygon2d.h>

#include <list>
#include <vector>

namespace geom {

std::list<Polygon2D> DecomposeToYMonotones(
    const std::vector<Point2D>& polygon_v);

}  // geom

#endif  // DECOMPOSE_TO_MONOTONES_H
