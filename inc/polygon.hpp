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
        std::string to_string();
};

#endif // #ifndef __POLYGON_HPP__