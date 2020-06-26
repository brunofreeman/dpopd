#include <GL/glew.h>
#include "vertex_buffer.hpp"
#include "renderer.hpp"

VertexBuffer::VertexBuffer(const void* data, size_t size) {
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &this->id);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}