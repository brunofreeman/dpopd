#ifndef __VERTEX_ARRAY_HPP__
#define __VERTEX_ARRAY_HPP__

#include "vertex_buffer_layout.hpp"

class VertexArray {
public:
    unsigned int id;

    VertexArray();

    ~VertexArray();

    void bind() const;

    void unbind() const;

    void add_buffer(const VertexBuffer* vertex_buffer, const VertexBufferLayout* layout);
};

#endif  // __VERTEX_ARRAY_HPP__