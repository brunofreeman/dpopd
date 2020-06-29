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

Environment* environment;
GraphicsObject* ego;
float padding = 0.05f;
Color environment_color(SILVER_RGB);

char glfw_version_major = 4;
char glfw_version_minor = 1;

short init_screen_width = 640;
short init_screen_height = 480;

char environment_name[] = "penta_in_hepta";
char window_name[] = "dpo_pdf";
char shader_path[] = "src/gfx/_monochrome.shader";

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

    glfw_set_version(glfw_version_major, glfw_version_minor);

    window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, glfw_window_resize_callback);

    return window;
}

static void glfw_window_resize_callback(GLFWwindow* window, int screen_width, int screen_height) {
    double x_scale = (1 - 2 * padding) * screen_width / (*environment).width;
    double y_scale = (1 - 2 * padding) * screen_height / (*environment).height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    #define SET_X (*ego).positions[index++] = scale * (2 * x - (*environment).width) / screen_width
    #define SET_Y (*ego).positions[index++] = scale * (2 * y - (*environment).height) / screen_height

    size_t index = 0;

    for (size_t i = 0; i < (*((*environment).border)).vertices_s; i++) {
        float x = (*((*environment).border)).vertices[i].x;
        float y = (*((*environment).border)).vertices[i].y;
        SET_X; SET_Y;
    }
    
    for (size_t i = 0; i < (*environment).obstacles_s; i++) {
        for (size_t j = 0; j < (*((*environment).obstacles[i])).vertices_s; j++) {
            float x = (*((*environment).obstacles[i])).vertices[j].x;
            float y = (*((*environment).obstacles[i])).vertices[j].y;
            SET_X; SET_Y;
        }
    }

    (*(*ego).vb).update_data((*ego).positions, (*ego).positions_s * sizeof(float));
}

static int show_visualization() {
    GLFWwindow* window = glfw_window_init(window_name, init_screen_width, init_screen_height);
    if (!window) return -1;
    if (glewInit() != GLEW_OK) return -1;
    gl_print_version();

    environment = json_environment(environment_name);
    ego = environment_graphics_object(environment, init_screen_width, init_screen_height, padding);
    Shader shader(shader_path);

    set_color(shader, environment_color);

    while (!glfwWindowShouldClose(window)) {
        clear();
        draw(ego, shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete environment;

    return 0;
}