#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "vertex_array.hpp"
#include "index_buffer.hpp"
#include "shader.hpp"
#include "environment.hpp"

struct RenderingBundle {
    VertexArray va;
    IndexBuffer ib;
    Shader shader;
};

void clear();
void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
RenderingBundle environment_rb(const Environment& environment);


#endif  // __RENDERER_HPP__