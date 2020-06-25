#include "polygon.hpp"

Polygon::Polygon(Vector* vertices, const size_t vertices_s) :
    vertices(vertices), vertices_s(vertices_s) {}

Polygon::~Polygon() {
    delete[] this->vertices;
}

std::string Polygon::to_string() {
    std::string poly_str = "[";

    for (size_t i = 0; i < this->vertices_s; i++){
        poly_str += this->vertices[i].to_string();
        if (i != this->vertices_s - 1) {
            poly_str += ", ";
        }
    }

    poly_str += "]";
    
    return poly_str;
}