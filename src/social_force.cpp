#include "social_force.hpp"
using namespace std;

SocialForce::~SocialForce() {
	this->delete_crowd();
	this->delete_walls();
}

void SocialForce::add_agent(Agent *agent) {
	this->crowd.push_back(agent);
}

void SocialForce::add_wall(Wall *wall) {
	this->walls.push_back(wall);
}

void SocialForce::delete_agent() {
	int last_idx;
	if (!this->crowd.empty()) {
		last_idx = this->crowd.size() - 1; // Assign index of last element
		delete this->crowd[last_idx];
		this->crowd.pop_back();
	}
}

void SocialForce::delete_crowd() {
	for (size_t i = 0; i < this->crowd.size(); i++) {
		delete this->crowd[i];
	}
	this->crowd.clear();
}

void SocialForce::delete_walls() {
	for (size_t i = 0; i < this->walls.size(); i++) {
		delete this->walls[i];
	}
	this->walls.clear();
}

void SocialForce::move_crowd(float step_time) {
	for (size_t i = 0; i < this->crowd.size(); i++) {
		this->crowd[i]->move(this->crowd, this->walls, step_time);
	}
}