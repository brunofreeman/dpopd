#ifndef __TO_STRING_HPP__
#define __TO_STRING_HPP__

#include <string>
#include "environment.hpp"
#include "polygon.hpp"
#include "vector.hpp"

std::string to_string(const Vector& vec);

std::string to_string(const Polygon& polygon);

std::string to_string(const Environment& environment);

#endif //__TO_STRING_HPP__