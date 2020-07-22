#include "sim/to_string.hpp"

std::string to_string(const Vector& vec) {
    return "(" + std::to_string(vec.x) + ", "
           + std::to_string(vec.y) + ")";
}

std::string to_string(const Polygon& polygon) {
    std::string poly_str = "[";

    for (size_t i = 0; i < polygon.vertices.size(); i++) {
        poly_str += to_string(polygon.vertices[i]);
        if (i != polygon.vertices.size() - 1) {
            poly_str += ", ";
        }
    }

    poly_str += "]";

    return poly_str;
}

std::string to_string(const Environment& environment) {
    std::string env_str = "b: ";
    env_str += to_string(*environment.border);

    env_str += "\no: {\n";
    for (size_t i = 0; i < environment.obstacles.size(); i++) {
        env_str += "     ";
        env_str += to_string(*environment.obstacles[i]);
        if (i != environment.obstacles.size() - 1) {
            env_str += ",";
        }
        env_str += "\n";
    }
    env_str += "}";

    return env_str;
}