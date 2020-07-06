#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "graphics_util.hpp"
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "vertex_array.hpp"
#include "shader.hpp"
#include "social_force.hpp"

static int show_visualization();
static void glfw_set_version(int major, int minor);
static GLFWwindow* glfw_window_init(const std::string& window_name, int width, int height);
static void glfw_window_resize_callback(GLFWwindow* window, int width, int height);
static void init_random();
static void create_walls();
static void create_agents();
static void add_polygon_walls(Polygon* polygon);

Environment* environment;
GraphicsObject* ego;
float padding = 0.05f;
Color environment_color(SILVER_RGB);

GraphicsObject** agos;
Color agent_color(ORANGE_RGB);

SocialForce* social_force;
size_t num_agents = 10;

char glfw_version_major = 4;
char glfw_version_minor = 1;

short screen_width = 640;
short screen_height = 480;

char environment_name[] = "penta_in_hepta";
char window_name[] = "dpo_pdf";
char shader_path[] = "src/gfx/_monochrome.shader";

int main(int argc, char** argv) {
    init_random();
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

static void glfw_window_resize_callback(GLFWwindow* window, int new_screen_width, int new_screen_height) {
    screen_width = new_screen_width;
    screen_height = new_screen_height;
    scale_environment_positions(ego, environment, screen_width, screen_height, padding);
    for (size_t i = 0; i < social_force->crowd.size(); i++) {
        scale_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
    }
}

static void init_random() {
    srand(time(NULL));
    srand48(time(NULL));
}

static void add_polygon_walls(Polygon* polygon) {
    size_t curr_idx = 0;
	do {
		size_t next_idx = (curr_idx + 1) % polygon->vertices_s;
        social_force->add_wall(new Wall(polygon->vertices[curr_idx], polygon->vertices[next_idx]));
		curr_idx = next_idx;
	} while (curr_idx != 0);
}

static void create_walls() {
    add_polygon_walls(environment->border);
    for (size_t i = 0; i < environment->obstacles_s; i++) {
        add_polygon_walls(environment->obstacles[i]);
    }
}

static void create_agents() {
    Agent* agent;
    for (size_t i = 0; i < num_agents; i++) {
        agent = new Agent();
        agent->position = environment->random_interior_point(agent->radius);
        agent->path.push_back((Waypoint){environment->random_interior_point(agent->radius), 3.0f});
        agent->update_shape();
        social_force->add_agent(agent);
    }
}

static int show_visualization() {
    GLFWwindow* window = glfw_window_init(window_name, screen_width, screen_height);
    if (!window) return -1;
    if (glewInit() != GLEW_OK) return -1;
    gl_print_version();

    Shader shader(shader_path);

    environment = json_environment(environment_name);
    ego = environment_graphics_object(environment, screen_width, screen_height, padding);

    social_force = new SocialForce();
    create_walls();
    create_agents();

    agos = new GraphicsObject*[social_force->crowd.size()];
    for (size_t i = 0; i < social_force->crowd.size(); i++) {
        agos[i] = agent_graphics_object(social_force->crowd[i], environment, screen_width, screen_height, padding);
    }

    auto prev_time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = curr_time - prev_time;
        prev_time = curr_time;
        social_force->move_crowd(elapsed_seconds.count());

        clear();

        set_color(shader, environment_color);
        draw(ego, shader);

        set_color(shader, agent_color);
        for (size_t i = 0; i < social_force->crowd.size(); i++) {
            social_force->crowd[i]->update_shape();
            agos[i]->obj = social_force->crowd[i]->shape;
            scale_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
            draw(agos[i], shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete environment;
    delete social_force;
    delete[] agos;

    return 0;
}