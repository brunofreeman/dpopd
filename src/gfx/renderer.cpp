#include <GL/glew.h>
#include <iostream>
#include "renderer.hpp"

void print_gl_version() {
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

void gl_clear_error() {
    while (glGetError() != GL_NO_ERROR);
}

void gl_check_error() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}