#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstdlib>
#include <vector>
#include "polygon.hpp"

/*
 * Defines the environment that agents will
 * navigate. Consists of a non-self-intersecting
 * polygon that serves as the border and 0 or more
 * holes.
 */
class Environment {
public:
    Polygon* border;
    std::vector<Polygon*> obstacles;
    double width;
    double height;

    // json_wrapper normalizes for rendering by making min(x) = 0 and min(y) = 0
    Environment(Polygon* border, std::vector<Polygon*> obstacles);

    ~Environment();

    bool is_interior_point(const Vector& point, double radius) const;

    // the point must be at least radius units away from all edges
    Vector random_interior_point(double radius) const;

    Vector random_interior_point() const;
};

#endif // #ifndef __ENVIRONMENT_HPP__