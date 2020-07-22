#ifndef __POLYGON_HPP__
#define __POLYGON_HPP__

#include <cstdlib>
#include <utility>
#include <vector>
#include "vector.hpp"

/*
 * Defines a non-self-intersecting polygon
 */
class Polygon {
public:
    std::vector<Vector> vertices;

    Polygon(std::vector<Vector> vertices) : vertices(std::move(vertices)) {}

    bool is_interior_point(const Vector& point, double radius) const;

    bool is_interior_point(const Vector& point) const;
};

/*
 * Returns a Polygon on the heap representing a regular n-gon
 * with radius r and centered at center
 */
Polygon* regular_ngon(const Vector& center, double r, size_t n);

#endif // #ifndef __POLYGON_HPP__