#include "vertex_buffer_layout.hpp"

void VertexBufferLayout::push_f(size_t count) {
    this->elements.push_back((VertexBufferElement) {GL_FLOAT, count, GL_FALSE});
    this->stride += count * VertexBufferElement::size(GL_FLOAT);
}

void VertexBufferLayout::push_ui(size_t count) {
    this->elements.push_back((VertexBufferElement) {GL_UNSIGNED_INT, count, GL_FALSE});
    this->stride += count * VertexBufferElement::size(GL_UNSIGNED_INT);
}

void VertexBufferLayout::push_c(size_t count) {
    this->elements.push_back((VertexBufferElement) {GL_UNSIGNED_BYTE, count, GL_TRUE});
    this->stride += count * VertexBufferElement::size(GL_UNSIGNED_BYTE);;
}