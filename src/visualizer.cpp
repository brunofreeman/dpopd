#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "graphics_util.hpp"
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "shader.hpp"
#include "move_model.hpp"
#include <bfreeman/dijkstra_polygon.hpp>

Environment* environment;
GraphicsObject* ego;
float padding = 0.05f;
Color environment_color(SILVER_RGB);

GraphicsObject** agos;
Color agent_color(ORANGE_RGB);

MoveModelType move_model_type = SOCIAL_FORCE_MODEL;
MoveModel* move_model;
size_t num_agents = 4;

char glfw_version_major = 4;
char glfw_version_minor = 1;

short screen_width = 640;
short screen_height = 480;

char environment_name[] = "penta_in_hepta_walls_1";
char window_name[] = "dpo_pdf";
char shader_path[] = "src/gfx/_monochrome.shader";

static void glfw_set_version(int major, int minor) {
    // 4.1 INTEL-14.6.18, could use lower #version if neccesary
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

static void glfw_window_resize_callback(GLFWwindow* window, int new_screen_width, int new_screen_height) {
    screen_width = new_screen_width;
    screen_height = new_screen_height;
    refresh_environment_positions(ego, environment, screen_width, screen_height, padding);
    for (size_t i = 0; i < move_model->crowd.size(); i++) {
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

static void init_random() {
    srand(time(nullptr));
    srand48(time(nullptr));
}

static void add_polygon_walls(Polygon* polygon) {
    size_t curr_idx = 0;
    do {
        size_t next_idx = (curr_idx + 1) % polygon->vertices.size();
        move_model->add_wall(new Wall(polygon->vertices[curr_idx], polygon->vertices[next_idx]));
        curr_idx = next_idx;
    } while (curr_idx != 0);
}

static void create_walls() {
    add_polygon_walls(environment->border);
    for (size_t i = 0; i < environment->obstacles.size(); i++) {
        add_polygon_walls(environment->obstacles[i]);
    }
}

static bfreeman::Point vec_to_point(const Vector& vec) {
    return (bfreeman::Point) {vec.x, vec.y};
}

static Vector point_to_vec(const bfreeman::Point& point) {
    return Vector(point.x, point.y);
}

static void populate_dijkstra_polygon(std::vector<std::vector<bfreeman::Point>>& dijkstra_polygon) {
    size_t dijkstra_polygon_idx = 0;
    for (size_t i = 0; i < environment->border->vertices.size(); i++) {
        dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(environment->border->vertices[i]));
    }
    for (size_t i = 0; i < environment->obstacles.size(); i++) {
        dijkstra_polygon_idx++;
        for (size_t j = 0; j < environment->obstacles[i]->vertices.size(); j++) {
            dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(environment->obstacles[i]->vertices[j]));
        }
    }
}

static void set_agent_waypoints(Agent* agent, const std::vector<std::vector<bfreeman::Point>>& dijkstra_polygon) {
    bfreeman::DijkstraData dd = bfreeman::dijkstra_path(dijkstra_polygon, vec_to_point(agent->position),
                                                        vec_to_point(
                                                                environment->random_interior_point(agent->radius)));
    for (size_t i = 1; i < dd.path.size(); i++) {
        agent->path.push_back((Waypoint) {point_to_vec(dd.path[i]), 3.0f});
    }
}

static void create_agents() {
    Agent* agent;
    std::vector<std::vector<bfreeman::Point>> dijkstra_polygon(environment->obstacles.size() + 1);
    populate_dijkstra_polygon(dijkstra_polygon);
    for (size_t i = 0; i < num_agents; i++) {
        agent = new Agent(move_model_type);
        agent->position = environment->random_interior_point(agent->radius);
        set_agent_waypoints(agent, dijkstra_polygon);
        agent->update_shape();
        move_model->add_agent(agent);
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

    move_model = new MoveModel();
    create_walls();
    create_agents();

    agos = new GraphicsObject* [move_model->crowd.size()];
    for (size_t i = 0; i < move_model->crowd.size(); i++) {
        agos[i] = agent_graphics_object(move_model->crowd[i], environment, screen_width, screen_height, padding);
    }

    auto prev_time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = curr_time - prev_time;
        prev_time = curr_time;

        switch (move_model_type) {
            case SOCIAL_FORCE_MODEL:
                move_model->sfm_move_crowd(elapsed_seconds.count());
                break;
        }

        clear();

        set_color(shader, environment_color);
        draw(ego, shader);

        set_color(shader, agent_color);
        for (size_t i = 0; i < move_model->crowd.size(); i++) {
            move_model->crowd[i]->update_shape();
            refresh_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
            draw(agos[i], shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete environment;
    delete move_model;
    delete[] agos;

    return 0;
}

int main(int argc, char** argv) {
    init_random();
    return show_visualization();
}