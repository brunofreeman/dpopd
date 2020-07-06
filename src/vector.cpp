#include <math.h>
#include "vector.hpp"

#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b
#define square(a) a*a

std::string Vector::to_string() {
    return "(" + std::to_string(this->x) + ", "
               + std::to_string(this->y) + ")";
}

Vector Vector::copy() const {
    return Vector(this->x, this->y);
}

void Vector::set(double x, double y) {
    this->x = x;
    this->y = y;
}

double Vector::norm_squared() const {
    return square(this->x) + square(this->y);
}

double Vector::norm() const {
    return sqrt(this->norm_squared());
}

void Vector::normalize() {
    double norm = this->norm();
    this->x /= norm;
    this->y /= norm;
}

void Vector::rotate(double angle) {
    double x = this->x * cos(angle) - this->y * sin(angle);
    double y = this->x * sin(angle) + this->y * cos(angle);
    this->x = x;
    this->y = y;
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

// returns the smallest angle between the vectors
double angle(const Vector& vec1, const Vector& vec2) {
    return acos(dot(vec1, vec2) / (vec1.norm() * vec2.norm()));
}

bool on_segment(const Vector& point, const Segment& seg) {
	return (point.x <= max(seg.p1.x, seg.p2.x) && point.x >= min(seg.p1.x, seg.p2.x)
            && point.y <= max(seg.p1.y, seg.p2.y) && point.y >= min(seg.p1.y, seg.p2.y));
}

Orientation orientation(const Vector& p, const Vector& q, const Vector& r) {
	int value = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (value == 0) return COLINEAR;
	return value > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
}

bool check_intersect(const Segment& seg1, const Segment& seg2)  {
	Orientation o1 = orientation(seg1.p1, seg1.p2, seg2.p1);
	Orientation o2 = orientation(seg1.p1, seg1.p2, seg2.p2);
	Orientation o3 = orientation(seg2.p1, seg2.p2, seg1.p1);
	Orientation o4 = orientation(seg2.p1, seg2.p2, seg1.p2);

	/* if (o1 != o2 && o3 != o4) return true;
	if (o1 == COLINEAR && on_segment(seg2.p1, seg1)) return true;
	if (o2 == COLINEAR && on_segment(seg2.p2, seg1)) return true;
	if (o3 == COLINEAR && on_segment(seg1.p1, seg2)) return true;
	if (o4 == COLINEAR && on_segment(seg1.p2, seg2)) return true;
    return false; */

    return o1 != o2 && o3 != o4;
}

double length(const Segment& seg) {
    return sqrt(
        square(seg.p2.x - seg.p1.x) +
        square(seg.p2.y - seg.p1.y)
    );
}

double distance(const Vector& point, const Segment& seg) {
    double distance = abs(
        (seg.p2.y - seg.p1.y) * point.x -
        (seg.p2.x - seg.p1.x) * point.y +
        seg.p2.x * seg.p1.y -
        seg.p2.y * seg.p1.x
    );
    distance /= length(seg);
    return distance;
}