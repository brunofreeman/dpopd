#include <utility>
#include "environment.hpp"

Environment::Environment(Polygon* border, std::vector<Polygon*> obstacles) :
        border(border), obstacles(std::move(obstacles)) {

    this->width = 0;
    this->height = 0;

    for (auto& vertex : border->vertices) {
        if (vertex.x > width) {
            this->width = vertex.x;
        }
        if (vertex.y > height) {
            this->height = vertex.y;
        }
    }
}

Environment::~Environment() {
    delete this->border;
    for (auto& obstacle : this->obstacles) {
        delete obstacle;
    }
}

bool Environment::is_interior_point(const Vector& point, double radius) const {
    if (!this->border->is_interior_point(point, radius)) return false;
    for (auto& obstacle : this->obstacles) {
        if (obstacle->is_interior_point(point, 0)) return false;
    }
    return true;
}

Vector Environment::random_interior_point(double radius) const {
    Vector point;
    do {
        point.set(drand48() * this->width, drand48() * this->height);
    } while (!this->is_interior_point(point, radius));
    return point;
}

Vector Environment::random_interior_point() const {
    return this->random_interior_point(0.0f);
}