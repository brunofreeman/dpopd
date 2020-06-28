#include <GL/glew.h>
#include <iostream>
#include "graphics_util.hpp"

void gl_clear_error() {
    while (glGetError() != GL_NO_ERROR);
}

bool gl_log_call(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void gl_print_version() {
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}