#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstdlib>
#include "polygon.hpp"

class Environment {
    public:
        Polygon *border;
        Polygon **obstacles;
        size_t obstacles_s;
        Environment(Polygon *border, Polygon **obstacles, const size_t obstacles_s);
};

#endif // #ifndef __ENVIRONMENT_HPP__