#include <random>
#include "agent.hpp"

int Agent::crowd_idx = -1;
std::default_random_engine generator;

Agent::Agent() {
	this->id = ++this->crowd_idx;
	
	this->radius = 0.2f;

	// Desired Speed Based on (Moussaid et al., 2009)
	std::normal_distribution<float> distribution(1.29f, 0.19f);	// Generate random value of mean 1.29 and standard deviation 0.19
	this->desired_speed = distribution(generator);

	this->shape = nullptr;
}

Agent::~Agent() {
	this->path.clear();				// Remove waypoints
	this->crowd_idx--;
	if (this->shape != nullptr) delete this->shape;
}

void Agent::update_shape() {
	if (this->shape != nullptr) delete this->shape;
    this->shape = regular_ngon(this->position, this->radius, this->shape_sides);
}

void Agent::push_waypoint(float x, float y, float radius) {
	this->path.push_back((Waypoint){ Vector(x, y), radius });
}

Vector Agent::immediate_goal() {
	Vector curr_dist, next_dist;

	curr_dist = this->path[0].position - this->position;			// Distance to current waypoint

	if (this->path.size() > 2) {
		next_dist = this->path[1].position - this->position;		// Distance to next waypoint

		// Set Next Waypoint as Current Waypoint if Next Waypoint is Nearer
		if (next_dist.norm_squared() < curr_dist.norm_squared()) {
			this->path.push_back(this->path.front());
			this->path.pop_front();

			curr_dist = next_dist;
		}
	}

	// Move Front Point to Back if Within Radius
	if (curr_dist.norm_squared() < (this->path.front().radius * this->path.front().radius)) {
		this->path.push_back(this->path.front());
		this->path.pop_front();
	}

	return this->path.front().position;
}

float Agent::orientation() const {
	return (atan2(this->velocity.y, this->velocity.x) * (180 / M_PI));
}

Vector Agent::ahead_vec() const {
	return (this->velocity + this->position);
}

void Agent::move(std::vector<Agent *> agents, std::vector<Wall *> walls, float step_time) {
	Vector acceleration;

	// Compute Social Force
	acceleration = this->driving_force(immediate_goal()) + this->agent_interaction_force(agents) + this->wall_interaction_force(walls);

	// Compute New Velocity
	this->velocity = this->velocity + acceleration * step_time;

	// Truncate Velocity if Exceed Maximum Speed (Magnitude)
	if (this->velocity.norm_squared() > (this->desired_speed * this->desired_speed)) {
		this->velocity.normalize();
		this->velocity *= this->desired_speed;
	}

	// Compute New Position
	this->position = this->position + this->velocity * step_time;
}

Vector Agent::driving_force(const Vector position_target) {
	const float T = 0.54f;	// Relaxation time based on (Moussaid et al., 2009)
	Vector e_i, f_i;

	// Compute Desired Direction
	// Formula: e_i = (position_target - position_i) / ||(position_target - position_i)||
	e_i = position_target - this->position;
	e_i.normalize();

	// Compute Driving Force
	// Formula: f_i = ((this->desired_speed * e_i) - velocity_i) / T
	f_i = ((this->desired_speed * e_i) - this->velocity) * (1 / T);

	return f_i;
}

Vector Agent::agent_interaction_force(std::vector<Agent *> agents) {
	// Constant Values Based on (Moussaid et al., 2009)
	const float lambda = 2.0;	// Weight reflecting relative importance of velocity vector against position vector
	const float gamma = 0.35f;	// Speed interaction
	const float n_prime = 3.0;	// Angular interaction
	const float n = 2.0;		// Angular intaraction
	const float A = 4.5;		// Modal parameter A

	Vector distance_ij, e_ij, D_ij, t_ij, n_ij, f_ij;
	float B, theta, f_v, f_theta;
	int K;

	f_ij.set(0.0, 0.0);

	for (const Agent *agent_j : agents) {
		// Do Not Compute Interaction Force to Itself
		if (agent_j->id != this->id) {
			// Compute Distance Between Agent j and i
			distance_ij = agent_j->position - this->position;

			// Skip Computation if Agents i and j are Too Far Away
			if (distance_ij.norm_squared() > (2.0 * 2.0))
				continue;

			// Compute Direction of Agent j from i
			// Formula: e_ij = (position_j - position_i) / ||position_j - position_i||
			e_ij = distance_ij;
			e_ij.normalize();

			// Compute Interaction Vector Between Agent i and j
			// Formula: D = lambda * (velocity_i - velocity_j) + e_ij
			D_ij = lambda * (this->velocity - agent_j->velocity) + e_ij;

			// Compute Modal Parameter B
			// Formula: B = gamma * ||D_ij||
			B = gamma * D_ij.norm();

			// Compute Interaction Direction
			// Formula: t_ij = D_ij / ||D_ij||
			t_ij = D_ij;
			t_ij.normalize();

			// Compute Angle Between Interaction Direction (t_ij) and Vector Pointing from Agent i to j (e_ij)
			theta = angle(t_ij, e_ij);

			// Compute Sign of Angle 'theta'
			// Formula: K = theta / |theta|
			K = (theta == 0) ? 0 : static_cast<int>(theta / abs(theta));

			// Compute Amount of Deceleration
			// Formula: f_v = -A * exp(-distance_ij / B - ((n_prime * B * theta) * (n_prime * B * theta)))
			f_v = -A * exp(-distance_ij.norm() / B - ((n_prime * B * theta) * (n_prime * B * theta)));

			// Compute Amount of Directional Changes
			// Formula: f_theta = -A * K * exp(-distance_ij / B - ((n * B * theta) * (n * B * theta)))
			f_theta = -A * K * exp(-distance_ij.norm() / B - ((n * B * theta) * (n * B * theta)));

			// Compute Normal Vector of Interaction Direction Oriented to the Left
			n_ij.set(-t_ij.y, t_ij.x);

			// Compute Interaction Force
			// Formula: f_ij = f_v * t_ij + f_theta * n_ij
			f_ij += f_v * t_ij + f_theta * n_ij;
		}
	}

	return f_ij;
}

Vector Agent::wall_interaction_force(std::vector<Wall *> walls) {
	//const float repulsionRange = 0.3f;	// Repulsion range based on (Moussaid et al., 2009)
	const int a = 3;
	const float b = 0.1f;

	Vector nearest_point;
	Vector vector_wi, minVector_wi;
	float square_dist, square_dist_min = INFINITY, d_w, f_iw;

	for (Wall *wall : walls) {
		nearest_point = wall->nearest_point(this->position);
		vector_wi = this->position - nearest_point;	// Vector from wall to agent i
		square_dist = vector_wi.norm_squared();

		// Store Nearest Wall Distance
		if (square_dist < square_dist_min) {
			square_dist_min= square_dist;
			minVector_wi = vector_wi;
		}
	}

	d_w = sqrt(square_dist_min) - this->radius;	// Distance between wall and agent i

	// Compute Interaction Force
	// Formula: f_iw = a * exp(-d_w / b)
	f_iw = a * exp(-d_w / b);
	minVector_wi.normalize();

	return f_iw * minVector_wi;
}