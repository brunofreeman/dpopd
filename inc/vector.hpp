#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

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

    explicit Vector(double x) : x(x), y(0) {}

    [[nodiscard]] Vector clone() const;

    void set(double x, double y);

    [[nodiscard]] double norm_squared() const;

    [[nodiscard]] double norm() const;

    void normalize();

    void rotate(double angle);

    void towards(const Vector& other, double distance);

    void away(const Vector& other, double distance);
};

struct Segment {
    Vector p1;
    Vector p2;
};

void operator+=(Vector& vec, const Vector& vec_const);

void operator-=(Vector& vec, const Vector& vec_const);

void operator*=(Vector& vec, const double& scalar);

void operator/=(Vector& vec, const double& scalar);

bool operator==(const Vector& vec1, const Vector& vec2);

Vector operator+(const Vector& vec1, const Vector& vec2);

Vector operator-(const Vector& vec1, const Vector& vec2);

Vector operator*(const Vector& vec, const double& scalar);

Vector operator*(const double& scalar, const Vector& vec);

double dot(const Vector& vec1, const Vector& vec2);

// returns the smallest angle between the vectors
double angle(const Vector& vec1, const Vector& vec2);

bool below_or_on_line(const Vector& point, const Segment& seg);

// for colinear points
bool on_segment(const Vector& point, const Segment& seg);

Orientation orientation(const Vector& p, const Vector& q, const Vector& r);

bool check_intersect(const Segment& seg1, const Segment& seg2);

bool check_intersect_no_endpoints(const Segment& seg1, const Segment& seg2);

double length(const Segment&);

double distance(const Vector& point, const Segment& seg);

// rotates around p1
Segment rotate_segment(const Segment& seg, double angle);

#endif // #ifndef __VECTOR_HPP__