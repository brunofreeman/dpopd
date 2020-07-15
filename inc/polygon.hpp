#ifndef __POLYGON_HPP__
#define __POLYGON_HPP__

#include <cstdlib>
#include <utility>
#include <vector>
#include "vector.hpp"

class Polygon {
public:
    std::vector<Vector> vertices;

    Polygon(std::vector<Vector> vertices) : vertices(std::move(vertices)) {}

    bool is_interior_point(const Vector& point, double radius) const;

    bool is_interior_point(const Vector& point) const;
};

Polygon* regular_ngon(const Vector& center, double r, size_t n);

#endif // #ifndef __POLYGON_HPP__