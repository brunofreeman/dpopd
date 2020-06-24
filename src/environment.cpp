#include "environment.hpp"

Environment::Environment(Polygon *border, Polygon **obstacles, const size_t obstacles_s) :
    border(border), obstacles(obstacles), obstacles_s(obstacles_s) {}