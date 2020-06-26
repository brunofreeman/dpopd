#include "renderer.hpp"
#include <GL/glew.h>
#include <iostream>

void gl_clear_error() {
    while (glGetError() != GL_NO_ERROR);
}

void gl_check_error() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}