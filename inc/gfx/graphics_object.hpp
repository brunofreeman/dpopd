#ifndef __GRAPHICS_OBJECT_HPP__
#define __GRAPHICS_OBJECT_HPP__

#include "vertex_array.hpp"
#include "index_buffer.hpp"

class GraphicsObject {
    public:
        VertexArray* va;
        VertexBuffer* vb;
        VertexBufferLayout* vbl;
        IndexBuffer* ib;
        float* positions;
        unsigned int* indices;
        size_t positions_s;
        size_t indices_s;
        GraphicsObject(VertexArray* va, VertexBuffer* vb, VertexBufferLayout* vbl, IndexBuffer* ib,
                       float* positions, unsigned int* indices, size_t positions_s, size_t indices_s) :
            va(va), vb(vb), vbl(vbl), ib(ib),
            positions(positions), indices(indices), positions_s(positions_s), indices_s(indices_s) {}
        ~GraphicsObject();
};

#endif // #ifndef __GRAPHICS_OBJECT_HPP__