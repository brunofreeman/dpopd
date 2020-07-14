#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstdlib>
#include <string>
#include "polygon.hpp"

class Environment {
public:
    Polygon* border;
    Polygon** obstacles;
    size_t obstacles_s;
    double width;
    double height;

    // json_wrapper normalizes for rendering by making min(x) = 0 and min(y) = 0
    Environment(Polygon* border, Polygon** obstacles, const size_t obstacles_s);

    ~Environment();

    bool is_interior_point(const Vector& point, double radius) const;

    Vector random_interior_point(double radius) const;

    Vector random_interior_point() const;

    std::string to_string() const;
};

#endif // #ifndef __ENVIRONMENT_HPP__