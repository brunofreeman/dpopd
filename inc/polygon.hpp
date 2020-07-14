#ifndef __POLYGON_HPP__
#define __POLYGON_HPP__

#include <cstdlib>
#include <string>
#include "vector.hpp"

class Polygon {
public:
    Vector* vertices;
    size_t vertices_s;

    Polygon(Vector* vertices, const size_t vertices_s);

    ~Polygon();

    bool is_interior_point(const Vector& point, double radius) const;

    bool is_interior_point(const Vector& point) const;

    std::string to_string() const;
};

Polygon* regular_ngon(const Vector& center, double r, size_t n);

#endif // #ifndef __POLYGON_HPP__