#include "sim/move_model.hpp"

MoveModel::~MoveModel() {
    this->delete_crowd();
    this->delete_walls();
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

void MoveModel::sfm_move_crowd(float step_time) {
    for (auto& agent : this->crowd) {
        agent->sfm_move(this->crowd, this->walls, step_time);
    }
}