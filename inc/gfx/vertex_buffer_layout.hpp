#ifndef __VERTEX_BUFFER_LAYOUT_HPP__
#define __VERTEX_BUFFER_LAYOUT_HPP__

#include <vector>
#include <GL/glew.h>
#include "vertex_buffer.hpp"

/*
 * Contains information on how data from
 * a VertexBuffer is structured (e.g.,
 * it contains a series of points each
 * consisting of two floats)
 */
struct VertexBufferElement {
    unsigned int type;
    size_t count;
    unsigned char normalized;

    static unsigned int size(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
            default:
                return 0;
        }
    }
};

class VertexBufferLayout {
public:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;

    VertexBufferLayout() : stride(0) {}

    ~VertexBufferLayout() = default;

    void push_f(size_t count);

    void push_ui(size_t count);

    void push_c(size_t count);
};

#endif  // __VERTEX_BUFFER_LAYOUT_HPP__