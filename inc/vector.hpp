#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <string>

enum Orientation {
    COUNTERCLOCKWISE = -1,
    COLINEAR = 0,
    CLOCKWISE = 1
};

class Vector {
    public:
        double x;
        double y;
        Vector() : x(0), y(0) {}
        Vector(double x, double y) : x(x), y(y) {}
        std::string to_string();
        Vector copy() const;
        void set(double x, double y);
        double norm_squared() const;
        double norm() const;
        void normalize();
        void rotate(double angle);
};

struct Segment {
    Vector p1;
    Vector p2;
};

void operator+=(Vector& vec, const Vector& vec_const);
void operator-=(Vector& vec, const Vector& vec_const);
void operator*=(Vector& vec, const double& scalar);
Vector operator+(const Vector& vec1, const Vector& vec2);
Vector operator-(const Vector& vec1, const Vector& vec2);
Vector operator*(const Vector& vec, const double& scalar);
Vector operator*(const double& scalar, const Vector& vec);
double dot(const Vector& vec1, const Vector& vec2);
// returns the smallest angle between the vectors
double angle(const Vector& vec1, const Vector& vec2);

bool on_segment(const Vector& point, const Segment& seg);
Orientation orientation(const Vector& p, const Vector& q, const Vector& r);
bool check_intersect(const Segment& seg1, const Segment& seg2);

double length(const Segment&);
double distance(const Vector& point, const Segment& seg);

#endif // #ifndef __VECTOR_HPP__