#include "color.hpp"

void Color::set(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Color::set(float r, float g, float b) {
    this->set(r, g, b, this->a);
}