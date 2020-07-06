#include "move_model.hpp"

MoveModel::~MoveModel() {
	this->delete_crowd();
	this->delete_walls();
}

void MoveModel::add_agent(Agent *agent) {
	this->crowd.push_back(agent);
}

void MoveModel::add_wall(Wall *wall) {
	this->walls.push_back(wall);
}

void MoveModel::delete_agent() {
	int last_idx;
	if (!this->crowd.empty()) {
		last_idx = this->crowd.size() - 1;
		delete this->crowd[last_idx];
		this->crowd.pop_back();
	}
}

void MoveModel::delete_crowd() {
	for (size_t i = 0; i < this->crowd.size(); i++) {
		delete this->crowd[i];
	}
	this->crowd.clear();
}

void MoveModel::delete_walls() {
	for (size_t i = 0; i < this->walls.size(); i++) {
		delete this->walls[i];
	}
	this->walls.clear();
}

void MoveModel::sfm_move_crowd(float step_time) {
	for (size_t i = 0; i < this->crowd.size(); i++) {
		this->crowd[i]->sfm_move(this->crowd, this->walls, step_time);
	}
}