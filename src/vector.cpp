#include <cmath>
#include "vector.hpp"

#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) > (b) ? (a) : (b)
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

void Vector::away(const Vector& other, const double distance) {
    Vector other_relative = other - *this;
    other_relative /= other_relative.norm();
    other_relative *= distance;
    *this -= other_relative;
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

// returns the smallest angle between the vectors
double angle(const Vector& vec1, const Vector& vec2) {
    double z = abs(vec1.x * vec2.y - vec1.y * vec2.x);
    return abs(atan2(z, dot(vec1, vec2)));
}

bool below_or_on_line(const Vector& point, const Segment& seg) {
    if (is_close(seg.p1.x, seg.p2.x)) return point.x <= seg.p1.x;
    double slope = (seg.p1.y - seg.p2.y) / (seg.p1.x - seg.p2.x);
    return point.y <= slope * point.x - slope * seg.p1.x + seg.p1.y;
}

bool on_segment(const Vector& point, const Segment& seg) {
    bool on = (point.x <= (max(seg.p1.x, seg.p2.x)) &&
               point.x >= (min(seg.p1.x, seg.p2.x)) &&
               point.y <= (max(seg.p1.y, seg.p2.y)) &&
               point.y >= (min(seg.p1.y, seg.p2.y)));
    bool endpoint = (point == seg.p1) || (point == seg.p2);
    return on || endpoint;
}

Orientation orientation(const Vector& p, const Vector& q, const Vector& r) {
    double value = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (is_close(value, 0)) return COLINEAR;
    return value > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
}

bool check_intersect(const Segment& seg1, const Segment& seg2) {
    Orientation o1 = orientation(seg1.p1, seg1.p2, seg2.p1);
    Orientation o2 = orientation(seg1.p1, seg1.p2, seg2.p2);
    Orientation o3 = orientation(seg2.p1, seg2.p2, seg1.p1);
    Orientation o4 = orientation(seg2.p1, seg2.p2, seg1.p2);

    return (o1 != o2 && o3 != o4) ||
           (o1 == COLINEAR && on_segment(seg2.p1, seg1)) ||
           (o2 == COLINEAR && on_segment(seg2.p2, seg1)) ||
           (o3 == COLINEAR && on_segment(seg1.p1, seg2)) ||
           (o4 == COLINEAR && on_segment(seg1.p2, seg2));
}

bool check_intersect_no_endpoints(const Segment& seg1, const Segment& seg2) {
    Orientation o1 = orientation(seg1.p1, seg1.p2, seg2.p1);
    Orientation o2 = orientation(seg1.p1, seg1.p2, seg2.p2);
    Orientation o3 = orientation(seg2.p1, seg2.p2, seg1.p1);
    Orientation o4 = orientation(seg2.p1, seg2.p2, seg1.p2);

    return o1 != o2 && o3 != o4;
}

double length(const Segment& seg) {
    return sqrt(
            sq(seg.p2.x - seg.p1.x) +
            sq(seg.p2.y - seg.p1.y)
    );
}

double distance(const Vector& point, const Segment& seg) {
    double len = length(seg);
    if (len == 0) return sqrt(sq(point.x - seg.p1.x) + sq(point.y - seg.p1.y));
    double distance = abs(
            (seg.p2.y - seg.p1.y) * point.x -
            (seg.p2.x - seg.p1.x) * point.y +
            seg.p2.x * seg.p1.y -
            seg.p2.y * seg.p1.x
    );
    distance /= len;
    return distance;
}

// rotates around p1
Segment rotate_segment(const Segment& seg, double angle) {
    Vector p1 = seg.p1.clone();
    Vector p2 = seg.p2 - seg.p1;
    p2.rotate(angle);
    p2 += p1;
    return (Segment) {p1, p2};
}