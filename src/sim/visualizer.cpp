#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "graphics_util.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include <bfreeman/dijkstra_polygon.hpp>

Environment* environment;
GraphicsObject* ego;
float padding = 0.05f;
Color environment_color(SILVER_RGB);

GraphicsObject** agos;
Color agent_pathing_color(ORANGE_RGB);
Color agent_stationary_color(GREEN_RGB);

MoveModelType move_model_type = SOCIAL_FORCE_MODEL;
MoveModel* move_model;
size_t num_agents = 20;

// radii given by Moussaid et al., 2009
double agent_radius = 0.2f;
double waypoint_radius = 3.0f;

char glfw_version_major = 4;
char glfw_version_minor = 1;

short screen_width = 640;
short screen_height = 480;

char environment_name[] = "penta_in_hepta_walls_2";
char window_name[] = "dpo_pdf";
char shader_path[] = "src/gfx/_monochrome.shader";

size_t min_stop_ticks = 100;
size_t max_stop_ticks = 500;

static void glfw_set_version(int major, int minor) {
    // 4.1 INTEL-14.6.18, could use lower #version if necessary
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

static void glfw_window_resize_callback(GLFWwindow* window, int new_screen_width, int new_screen_height) {
    screen_width = new_screen_width;
    screen_height = new_screen_height;
    refresh_environment_positions(ego, environment, screen_width, screen_height, padding);
    for (size_t i = 0; i < move_model->agents.size(); i++) {
        refresh_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
    }
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

static int show_visualization() {
    GLFWwindow* window = glfw_window_init(window_name, screen_width, screen_height);
    if (!window) return -1;
    if (glewInit() != GLEW_OK) return -1;
    gl_print_version();

    Shader shader(shader_path);

    environment = json_environment(environment_name);
    ego = environment_graphics_object(environment, screen_width, screen_height, padding);

    move_model = new MoveModel(SOCIAL_FORCE_MODEL, RANDOM, environment,
            num_agents, agent_radius, waypoint_radius,
            min_stop_ticks, max_stop_ticks);

    agos = new GraphicsObject* [move_model->agents.size()];
    for (size_t i = 0; i < move_model->agents.size(); i++) {
        agos[i] = agent_graphics_object(move_model->agents[i], environment, screen_width, screen_height, padding);
    }

    auto prev_time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = curr_time - prev_time;
        prev_time = curr_time;

        move_model->move_crowd(elapsed_seconds.count());

        clear();

        set_color(shader, environment_color);
        draw(ego, shader);

        for (size_t i = 0; i < move_model->agents.size(); i++) {
            move_model->agents[i]->update_shape();
            if (move_model->agents[i]->is_pathing) set_color(shader, agent_pathing_color);
            else set_color(shader, agent_stationary_color);
            refresh_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
            draw(agos[i], shader);
            if (move_model->agents[i]->needs_repathing) {
                move_model->set_agent_waypoints(move_model->agents[i], move_model->agents[i]->path.back().position);
                move_model->agents[i]->needs_repathing = false;
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete move_model;
    delete ego;
    delete[] agos;

    return 0;
}

int main(int argc, char** argv) {
    init_random(time(nullptr), time(nullptr), time(nullptr));
    return show_visualization();
}