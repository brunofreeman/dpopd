#include <cmath>
#include "vector.hpp"

#define sq(a) (a)*(a)
#define DBL_EPSILON 10e-7
#define is_close(a, b) abs((a) - (b)) < DBL_EPSILON

Vector Vector::clone() const {
    return Vector(this->x, this->y);
}

void Vector::set(double x, double y) {
    this->x = x;
    this->y = y;
}

double Vector::norm_squared() const {
    return sq(this->x) + sq(this->y);
}

double Vector::norm() const {
    return sqrt(this->norm_squared());
}

void Vector::normalize() {
    double norm = this->norm();
    if (is_close(norm, 0)) return;
    this->x /= norm;
    this->y /= norm;
}

void Vector::rotate(double angle) {
    double x = this->x * cos(angle) - this->y * sin(angle);
    double y = this->x * sin(angle) + this->y * cos(angle);
    this->x = x;
    this->y = y;
}

void Vector::towards(const Vector& other, const double distance) {
    Vector other_relative = other - *this;
    other_relative /= other_relative.norm();
    other_relative *= distance;
    *this += other_relative;
}

void operator+=(Vector& vec, const Vector& vec_const) {
    vec.x += vec_const.x;
    vec.y += vec_const.y;
}

void operator-=(Vector& vec, const Vector& vec_const) {
    vec.x -= vec_const.x;
    vec.y -= vec_const.y;
}

void operator*=(Vector& vec, const double& scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
}

void operator/=(Vector& vec, const double& scalar) {
    vec.x /= scalar;
    vec.y /= scalar;
}

bool operator==(const Vector& vec1, const Vector& vec2) {
    return is_close(vec1.x, vec2.x) && is_close(vec1.y, vec2.y);
}

Vector operator+(const Vector& vec1, const Vector& vec2) {
    return Vector(vec1.x + vec2.x, vec1.y + vec2.y);
}

Vector operator-(const Vector& vec1, const Vector& vec2) {
    return Vector(vec1.x - vec2.x, vec1.y - vec2.y);
}

Vector operator*(const Vector& vec, const double& scalar) {
    return Vector(scalar * vec.x, scalar * vec.y);
}

Vector operator*(const double& scalar, const Vector& vec) {
    return Vector(scalar * vec.x, scalar * vec.y);
}

double dot(const Vector& vec1, const Vector& vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

double angle(const Vector& vec1, const Vector& vec2) {
    double z = abs(vec1.x * vec2.y - vec1.y * vec2.x);
    return abs(atan2(z, dot(vec1, vec2)));
}