#ifndef DECOMPOSE_TO_MONOTONES_H
#define DECOMPOSE_TO_MONOTONES_H

#include <geom_utils.h>
#include <polygon2d.h>

#include <list>
#include <vector>

namespace geom {

// returns std::list to avoid redundant Polygon2D copying
// since Polygon2D construction is extremely costly
// and it's hard to precalculate number of result y-monotones
// since DcelPolygon2D stores redundant faces
std::list<Polygon2D> DecomposeToYMonotones(
    const std::vector<Point2D>& polygon_v);

}  // geom

#endif  // DECOMPOSE_TO_MONOTONES_H
