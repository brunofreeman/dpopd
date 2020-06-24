#include <math.h>
#include "vector.hpp"

Vector::Vector(const double x, const double y) : x(x), y(y) {}

Vector Vector::copy() const {
    return Vector(x, y);
}

void Vector::set(const double x_, const double y_) {
    x = x_;
    y = y_;
}

double Vector::norm_squared() const {
    return  x*x + y*y;
}

double Vector::norm() const {
    return sqrt(this->norm_squared());
}

void Vector::normalize() {
    double norm = this->norm();
    x /= norm;
    y /= norm;
}

void operator+=(Vector &vec, const Vector &vec_const) {
    vec.x += vec_const.x;
    vec.y += vec_const.y;
}

void operator-=(Vector &vec, const Vector &vec_const) {
    vec.x -= vec_const.x;
    vec.y -= vec_const.y;
}

void operator*=(Vector &vec, const double &scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
}

Vector operator+(const Vector &vec1, const Vector &vec2) {
    return Vector(vec1.x + vec2.x, vec1.y + vec2.y);
}

Vector operator-(const Vector &vec1, const Vector &vec2) {
    return Vector(vec1.x - vec2.x, vec1.y - vec2.y);
}

Vector operator*(const Vector &vec, const double &scalar) {
    return Vector(scalar * vec.x, scalar * vec.y);
}

Vector operator*(const double &scalar, const Vector &vec) {
    return Vector(scalar * vec.x, scalar * vec.y);
}

double dot(const Vector &vec1, const Vector &vec2) {
    return vec1.x*vec2.x + vec1.y*vec2.y;
}

// returns the smallest angle between the vectors
double angle(const Vector &vec1, const Vector &vec2) {
    return acos(dot(vec1, vec2) / (vec1.norm() * vec2.norm()));
}