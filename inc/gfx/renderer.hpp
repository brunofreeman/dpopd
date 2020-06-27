#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "vertex_array.hpp"
#include "index_buffer.hpp"
#include "shader.hpp"

void clear();
void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);

#endif  // __RENDERER_HPP__