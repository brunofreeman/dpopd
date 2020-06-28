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
        // assumed to be all positive coords, min(x) = 0, min(y) = 0
        Environment(Polygon* border, Polygon** obstacles, const size_t obstacles_s);
        ~Environment();
        std::string to_string();
};

#endif // #ifndef __ENVIRONMENT_HPP__