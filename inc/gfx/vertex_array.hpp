#ifndef __VERTEX_ARRAY_HPP__
#define __VERTEX_ARRAY_HPP__

#include "vertex_buffer_layout.hpp"

/*
 * Contains information about vertex positions,
 * specifically (x, y) coordinates on a [-1.0, 1.0]
 * scale, representing proportions of the application
 * window
 */
class VertexArray {
public:
    unsigned int id;

    VertexArray();

    ~VertexArray();

    void bind() const;

    void unbind() const;

    // adds both the data and info on how the data is structure
    void add_buffer(const VertexBuffer* vertex_buffer, const VertexBufferLayout* layout);
};

#endif  // __VERTEX_ARRAY_HPP__