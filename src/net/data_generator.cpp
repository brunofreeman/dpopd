#include "json_wrapper.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include <bfreeman/dijkstra_polygon.hpp>
#include <iostream>
#include "to_string.hpp"

const double time_step = 0.01;
const std::string model_name = "first";

Model* model;
Environment* environment;

std::vector<std::vector<bfreeman::Point>> dijkstra_polygon;

MoveModelType move_model_type = SOCIAL_FORCE_MODEL;
MoveModel* move_model;
size_t num_agents = 20;

// radii given by Moussaid et al., 2009
float agent_radius = 0.2f;
float waypoint_radius = 3.0f;

static void add_polygon_walls(const Polygon* polygon) {
    size_t curr_idx = 0;
    do {
        size_t next_idx = (curr_idx + 1) % polygon->vertices.size();
        move_model->add_wall(new Wall(polygon->vertices[curr_idx], polygon->vertices[next_idx]));
        curr_idx = next_idx;
    } while (curr_idx != 0);
}

static void create_walls() {
    add_polygon_walls(environment->border);
    for (auto& obstacle : environment->obstacles) {
        add_polygon_walls(obstacle);
    }
}

static bfreeman::Point vec_to_point(const Vector& vec) {
    return (bfreeman::Point) {vec.x, vec.y};
}

static void populate_dijkstra_polygon() {
    dijkstra_polygon = std::vector<std::vector<bfreeman::Point>>(environment->obstacles.size() + 1);
    size_t dijkstra_polygon_idx = 0;
    for (auto& vertex : environment->border->vertices) {
        dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(vertex));
    }
    for (auto& obstacle : environment->obstacles) {
        dijkstra_polygon_idx++;
        for (auto& vertex : obstacle->vertices) {
            dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(vertex));
        }
    }
}

static void set_agent_waypoints(Agent* agent, const Vector& goal) {
    bfreeman::DijkstraData dd = bfreeman::dijkstra_path(dijkstra_polygon, vec_to_point(agent->position),
                                                        vec_to_point(goal));
    agent->path.clear();
    for (size_t i = 1; i < dd.path.size(); i++) {
        agent->push_waypoint(dd.path[i].x, dd.path[i].y, waypoint_radius);
    }
}

static void create_agents() {
    Agent* agent;
    for (size_t i = 0; i < num_agents; i++) {
        agent = new Agent(move_model_type, agent_radius);
        agent->position = environment->random_interior_point(agent->radius);
        set_agent_waypoints(agent, environment->random_interior_point(agent->radius));
        agent->update_corner_direction();
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
    populate_dijkstra_polygon();
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

        for (size_t i = 0; i < move_model->crowd.size(); i++) {
            move_model->crowd[i]->update_shape();
            if (move_model->crowd[i]->is_pathing) set_color(shader, agent_pathing_color);
            else set_color(shader, agent_stationary_color);
            refresh_polygon_positions(agos[i], environment, screen_width, screen_height, padding);
            draw(agos[i], shader);
            if (move_model->crowd[i]->needs_repathing) {
                set_agent_waypoints(move_model->crowd[i], move_model->crowd[i]->path.back().position);
                move_model->crowd[i]->needs_repathing = false;
            }
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
    model = json_model(model_name);
    environment = json_environment(model->environment_name);
    std::cout << model->environment_name << ", " << model->crowd_type <<
                ", " << model->spacial_res << ", " << model->temporal_res << std::endl;
    std::cout << to_string(*environment) << std::endl;

    init_random(time(nullptr), time(nullptr), time(nullptr));
    return show_visualization();
}
