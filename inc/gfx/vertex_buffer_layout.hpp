#ifndef __VERTEX_BUFFER_LAYOUT_HPP__
#define __VERTEX_BUFFER_LAYOUT_HPP__

#include <vector>
#include "vertex_buffer.hpp"

struct VertexBufferElement {
    unsigned int type;
    size_t count;
    bool normalized;
};

class VertexBufferLayout {
    public:
        std::vector<VertexBufferElement> elements;
        unsigned int stride;
        VertexBufferLayout() : stride(0) {}
        ~VertexBufferLayout();

        template<typename T>
        void push(int count) {
            static_assert(false);
        }

        template<>
        void push<float>(int count) {
            this->elements.push_back((VertexBufferElement){GL_FLOAT, count, false});
            stride += 4;
        }

        template<>
        void push<unsigned int>(int count) {
            this->elements.push_back((VertexBufferElement){GL_UNSIGNED_INT, count, false});
        }

        template<>
        void push<unsigned char>(int count) {
            this->elements.push_back((VertexBufferElement){GL_UNSIGNED_BYTE, count, true});
        }
};

#endif  // __VERTEX_BUFFER_LAYOUT_HPP__