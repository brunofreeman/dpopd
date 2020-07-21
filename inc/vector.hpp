#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

/*
 * A vector in 2D space
 */
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

    // rotates about the origin
    void rotate(double angle);

    /*
     * Moves the head of the vector towards the
     * head of the other vector by the specified
     * distance
     */
    void towards(const Vector& other, double distance);
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

#endif // #ifndef __VECTOR_HPP__