#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "gfx_util.hpp"
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "vertex_array.hpp"
#include "shader.hpp"

static int make_window();

int main(int argc, char** argv) {
    Environment* environment = json_environment("two_sqr_in_sqr");

    std::cout << (*environment).to_string() << std::endl;

    delete environment;

    make_window();

    return 0;
}

static int make_window() {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    // 4.1 INTEL-14.6.18, could use lower #version if neccesary
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(640, 480, "dpo_pdf", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    gl_print_version();

    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f  // 3
    };

    unsigned int indices[] {
        0, 1, 2,
        2, 3, 0
    };

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.push_f(2);
    va.add_buffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("src/gfx/_default.shader");
    shader.bind();

    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

    float r = 0.0f;
    float inc = 0.05f;

    while (!glfwWindowShouldClose(window)) {
        clear();

        shader.bind();
        shader.set_uniform_4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        draw(va, ib, shader);

        if (r > 1) inc = -0.05f;
        else if (r < 0) inc = 0.05f;
        r += inc;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}