#include "environment.hpp"

#include <utility>

Environment::Environment(Polygon* border, std::vector<Polygon*> obstacles) :
        border(border), obstacles(std::move(obstacles)) {

    this->width = 0;
    this->height = 0;

    for (size_t i = 0; i < border->vertices.size(); i++) {
        if (border->vertices[i].x > width) {
            this->width = border->vertices[i].x;
        }
        if (border->vertices[i].y > height) {
            this->height = border->vertices[i].y;
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
    for (auto obstacle : this->obstacles) {
        if (obstacle->is_interior_point(point, radius)) return false;
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

std::string Environment::to_string() const {
    std::string env_str = "b: ";
    env_str += this->border->to_string();

    env_str += "\no: {\n";
    for (size_t i = 0; i < obstacles.size(); i++) {
        env_str += "     ";
        env_str += this->obstacles[i]->to_string();
        if (i != this->obstacles.size() - 1) {
            env_str += ",";
        }
        env_str += "\n";
    }
    env_str += "}";

    return env_str;
}