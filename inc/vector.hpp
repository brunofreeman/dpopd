#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <string>

class Vector {
    public:
        double x;
        double y;
        Vector() : x(0), y(0) {}
        Vector(const double, const double);
        std::string to_string();
        Vector copy() const;
        void set(const double x, const double y);
        double norm_squared() const;
        double norm() const;
        void normalize();
};

void operator+=(Vector &vec, const Vector &vec_const);
void operator-=(Vector &vec, const Vector &vec_const);
void operator*=(Vector &vec, const double &scalar);
Vector operator+(const Vector &vec1, const Vector &vec2);
Vector operator-(const Vector &vec1, const Vector &vec2);
Vector operator*(const Vector &vec, const double &scalar);
Vector operator*(const double &scalar, const Vector &vec);
double dot(const Vector &vec1, const Vector &vec2);
// returns the smallest angle between the vectors
double angle(const Vector &vec1, const Vector &vec2);

#endif // #ifndef __VECTOR_HPP__