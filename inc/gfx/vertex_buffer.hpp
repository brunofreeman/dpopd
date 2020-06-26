#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__

#include <cstdlib>

class VertexBuffer {
    public:
        unsigned int id;
        VertexBuffer(const void* data, size_t size);
        ~VertexBuffer();
        void bind() const;
        void unbind() const;
};

#endif  // __VERTEX_BUFFER_HPP__