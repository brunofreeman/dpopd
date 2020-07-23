#include "move_model.hpp"

MoveModel::MoveModel(MoveModelType type, SpawnGoalType spawn_goal_type,
                     Environment *environment, size_t num_agents, double agent_radius, double waypoint_radius) {

    this->type = type;
    this->agent_radius = agent_radius;
    this->waypoint_radius = waypoint_radius;
    this->environment = environment;
    this->populate_dijkstra_polygon();
    this->create_walls();
    this->create_agents(num_agents);

}

MoveModel::~MoveModel() {
    this->delete_crowd();
    this->delete_walls();
    delete this->environment;
}

bool MoveModel::pathing() {
    for (auto& agent : this->crowd) {
        if (agent->is_pathing) return true;
    }
    return false;
}

void MoveModel::add_agent(Agent* agent) {
    this->crowd.push_back(agent);
}

void MoveModel::add_wall(Wall* wall) {
    this->walls.push_back(wall);
}

void MoveModel::delete_agent() {
    size_t last_idx;
    if (!this->crowd.empty()) {
        last_idx = this->crowd.size() - 1;
        delete this->crowd[last_idx];
        this->crowd.pop_back();
    }
}

void MoveModel::delete_crowd() {
    for (auto& agent : this->crowd) {
        delete agent;
    }
    this->crowd.clear();
}

void MoveModel::delete_walls() {
    for (auto& wall : this->walls) {
        delete wall;
    }
    this->walls.clear();
}

void MoveModel::move_crowd(double step_time) {
    switch (this->type) {
        case SOCIAL_FORCE_MODEL:
            this->sfm_move_crowd(step_time);
            break;
    }
}

void MoveModel::sfm_move_crowd(double step_time) {
    for (auto& agent : this->crowd) {
        agent->sfm_move(this->crowd, this->walls, step_time);
    }
}

void MoveModel::add_polygon_walls(const Polygon* polygon) {
    size_t curr_idx = 0;
    do {
        size_t next_idx = (curr_idx + 1) % polygon->vertices.size();
        this->add_wall(new Wall(polygon->vertices[curr_idx], polygon->vertices[next_idx]));
        curr_idx = next_idx;
    } while (curr_idx != 0);
}

void MoveModel::create_walls() {
    this->add_polygon_walls(environment->border);
    for (auto& obstacle : environment->obstacles) {
        this->add_polygon_walls(obstacle);
    }
}

static bfreeman::Point vec_to_point(const Vector& vec) {
    return (bfreeman::Point) {vec.x, vec.y};
}

void MoveModel::populate_dijkstra_polygon() {
    this->dijkstra_polygon = std::vector<std::vector<bfreeman::Point>>(environment->obstacles.size() + 1);
    size_t dijkstra_polygon_idx = 0;
    for (auto& vertex : this->environment->border->vertices) {
        this->dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(vertex));
    }
    for (auto& obstacle : this->environment->obstacles) {
        dijkstra_polygon_idx++;
        for (auto& vertex : obstacle->vertices) {
            this->dijkstra_polygon[dijkstra_polygon_idx].push_back(vec_to_point(vertex));
        }
    }
}

void MoveModel::set_agent_waypoints(Agent* agent, const Vector& goal) const {
    bfreeman::DijkstraData dd = bfreeman::dijkstra_path(this->dijkstra_polygon, vec_to_point(agent->position),
                                                        vec_to_point(goal));
    agent->path.clear();
    for (size_t i = 1; i < dd.path.size(); i++) {
        agent->push_waypoint(dd.path[i].x, dd.path[i].y, this->waypoint_radius);
    }
}

void MoveModel::create_agents(size_t num_agents) {
    Agent* agent;
    for (size_t i = 0; i < num_agents; i++) {
        agent = new Agent(this->agent_radius);
        agent->position = environment->random_interior_point(agent->radius);
        this->set_agent_waypoints(agent, environment->random_interior_point(agent->radius));
        agent->update_corner_direction();
        agent->update_shape();
        this->add_agent(agent);
    }
}