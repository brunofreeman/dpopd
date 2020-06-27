#include <iostream>
#include <GL/glew.h>
#include "renderer.hpp"

void clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) {
    shader.bind();
    va.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, nullptr);
}