#include "graphics_object.hpp"

GraphicsObject::~GraphicsObject() {
    delete this->va;
    delete this->vb;
    delete this->vbl;
    delete this->ib;
    delete[] this->positions;
    delete[] this->indices;
}