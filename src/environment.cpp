#include "environment.hpp"

Environment::Environment(Polygon* border, Polygon** obstacles, const size_t obstacles_s) :
    border(border), obstacles(obstacles), obstacles_s(obstacles_s) {

    double min_x =  __DBL_MAX__;
    double min_y =  __DBL_MAX__;
    double max_x = -__DBL_MAX__;
    double max_y = -__DBL_MAX__;
    
    for (size_t i = 0; i < (*border).vertices_s; i++) {
        if ((*border).vertices[i].x < min_x) {
            min_x = (*border).vertices[i].x;
        } else if ((*border).vertices[i].x > max_x) {
            max_x = (*border).vertices[i].x;
        }

        if ((*border).vertices[i].y < min_y) {
            min_y = (*border).vertices[i].y;
        } else if ((*border).vertices[i].y > max_y) {
            max_y = (*border).vertices[i].y;
        }
    }

    this->extremes[0] = min_x;
    this->extremes[1] = min_y;
    this->extremes[2] = max_x;
    this->extremes[3] = max_y;
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