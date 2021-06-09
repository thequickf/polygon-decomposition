#ifndef TRIAGULATION_EXPOSE_TRIANGULATION_H
#define TRIAGULATION_EXPOSE_TRIANGULATION_H

#include <triangulation_base_geometry.h>

#include <vector>

namespace geom {

std::vector<Triangle2D> Triangulate(const std::vector<Point2D>& polygon);

}  // geom

#endif  // TRIAGULATION_EXPOSE_TRIANGULATION_H
