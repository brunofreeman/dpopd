#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__

#include <cstdlib>

/*
 * Contains the raw data for a VertexArray,
 * but does not contain info on how that data
 * should be read
 */
class VertexBuffer {
public:
    unsigned int id;

    VertexBuffer(const void* data, size_t size);

    ~VertexBuffer();

    void bind() const;

    void unbind() const;
};

void set_vb_data(const void* data, size_t size);

#endif  // __VERTEX_BUFFER_HPP__