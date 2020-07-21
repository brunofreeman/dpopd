#include <GL/glew.h>
#include "index_buffer.hpp"
#include "renderer.hpp"

IndexBuffer::IndexBuffer(const void* data, size_t count) : count(count) {
    glGenBuffers(1, &this->id); // assigned the ID
    this->bind();
    // binds the index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &this->id);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}