#include "vertex_array.hpp"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &this->id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &this->id);
}

void VertexArray::bind() const {
    glBindVertexArray(this->id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::add_buffer(const VertexBuffer& vertex_buffer, const VertexBufferLayout& layout) {
    this->bind();
    vertex_buffer.bind();
    const auto& elements = layout.elements;
    unsigned int offset = 0;
    for (size_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride, (const void*)offset);
        offset += element.count * VertexBufferElement::size(element.type);
    }
}