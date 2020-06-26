#ifndef __VERTEX_ARRAY_HPP__
#define __VERTEX_ARRAY_HPP__

#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

class VertexArray {
    public:
        VertexArray();
        ~VertexArray();
        void add_buffer(const VertexBuffer& vertex_buffer, const VertexBufferLayout& layout);
};

#endif  // __VERTEX_ARRAY_HPP__