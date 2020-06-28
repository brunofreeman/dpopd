#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "graphics_util.hpp"
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "vertex_array.hpp"
#include "shader.hpp"

static int show_visualization();
static void glfw_set_version(int major, int minor);
static GLFWwindow* glfw_window_init(const std::string& window_name, int width, int height);
static void glfw_window_resize_callback(GLFWwindow* window, int width, int height);

int main(int argc, char** argv) {
    return show_visualization();
}

static void glfw_set_version(int major, int minor) {
    // 4.1 INTEL-14.6.18, could use lower #version if neccesary
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

static GLFWwindow* glfw_window_init(const std::string& window_name, int width, int height) {
    GLFWwindow* window;

    if (!glfwInit()) return nullptr;

    glfw_set_version(4, 1);

    window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, glfw_window_resize_callback);

    return window;
}

static void glfw_window_resize_callback(GLFWwindow* window, int width, int height) {
    std::cout << "(" << width << ", " << height << ")" << std::endl;
}

static int show_visualization() {
    Environment* environment = json_environment("two_sqr_in_sqr");

    int init_width = 640;
    int init_height = 480;
    GLFWwindow* window = glfw_window_init("dpo_pdf", init_width, init_height);

    if (!window) return -1;
    if (glewInit() != GLEW_OK) return -1;

    gl_print_version();

    GraphicsObject ego = environment_graphics_object(environment, init_width, init_height, 0.05f);

    Shader shader("src/gfx/_default.shader");

    float r = 0.0f;
    float inc = 0.05f;

    while (!glfwWindowShouldClose(window)) {
        clear();

        shader.bind();
        shader.set_uniform_4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        // draw(va, ib, shader);
        draw(ego, shader);

        if (r > 1) inc = -0.05f;
        else if (r < 0) inc = 0.05f;
        r += inc;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete environment;

    return 0;
}