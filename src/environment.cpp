#include "environment.hpp"

Environment::Environment(Polygon* border, Polygon** obstacles, const size_t obstacles_s) :
    border(border), obstacles(obstacles), obstacles_s(obstacles_s) {

    this->width = 0;
    this->height = 0;
    
    for (size_t i = 0; i < (*border).vertices_s; i++) {
        if ((*border).vertices[i].x > width) {
            this->width = (*border).vertices[i].x;
        }
        if ((*border).vertices[i].y > height) {
            this->height = (*border).vertices[i].y;
        }
    }
}

Environment::~Environment() {
    delete this->border;
    for (size_t i = 0; i < this->obstacles_s; i++) {
        delete this->obstacles[i];
    }
    delete[] this->obstacles;
}

std::string Environment::to_string() {
    std::string env_str = "b: ";
    env_str += (*this->border).to_string();

    env_str += "\no: {\n";
    for (size_t i = 0; i < obstacles_s; i++) {
        env_str += "     ";
        env_str += (*this->obstacles[i]).to_string();
        if (i != this->obstacles_s - 1) {
            env_str += ",";
        }
        env_str += "\n";
    }
    env_str += "}";

    return env_str;
}