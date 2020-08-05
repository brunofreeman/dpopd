#include <cmath>
#include "agent.hpp"
#include "random_wrapper.hpp"

size_t Agent::crowd_size = 0;


Agent::Agent(const double radius) : radius(radius), is_pathing(true),
                                    at_corner(false), shape(nullptr),
                                    corner_direction(CLOCKWISE),
                                    needs_repathing(false), stuck_ticks(0) {

    this->id = Agent::crowd_size++;
    this->desired_speed = from_normal_distribution(1.29, 0.19);
    this->stopped_ticks = 0;
}

Agent::~Agent() {
    this->path.clear();
    Agent::crowd_size--;
    delete this->shape;
}

void Agent::update_shape() {
    if (this->shape == nullptr) {
        this->shape = regular_ngon(this->position, this->radius, Agent::shape_sides);
    } else {
        for (size_t i = 0; i < Agent::shape_sides; i++) {
            this->shape->vertices[i].x += this->position.x - this->prev_update_pos.x;
            this->shape->vertices[i].y += this->position.y - this->prev_update_pos.y;
        }
    }
    this->prev_update_pos = this->position;
}

void Agent::push_waypoint(const double x, const double y, const double waypoint_radius) {
    this->path.push_back((Waypoint) {Vector(x, y), waypoint_radius});
}

Segment wall_to_seg(const Wall& wall) {
    return (Segment) {
            (Vector) {wall.wall.start.x, wall.wall.start.y},
            (Vector) {wall.wall.end.x, wall.wall.end.y}
    };
}

bool Agent::clear_path_to(const Vector& to, const std::vector<Wall*>& walls) const {
    size_t num_points = 4;
    for (size_t i = 0; i < num_points; i++) {
        auto point = Vector(this->radius);
        point.rotate(2 * M_PI * i / num_points);
        point += this->position;
        auto seg = (Segment) {point, to};
        for (auto& wall : walls) {
            if (check_intersect_no_endpoints(seg, wall_to_seg(*wall))) {
                return false;
            }
        }
    }
    return true;
}

void Agent::update_corner_direction() {
    this->corner_direction = orientation(this->position, this->path[0].position, this->path[1].position);
}

void Agent::refresh_immediate_goal(const std::vector<Wall*>& walls) {
    Vector distance = this->path.front().position - this->position;
    double dist = distance.norm();
    bool in_waypoint = dist < this->path.front().radius;
    bool near_center = dist < this->radius * 1.5;

    if (this->path.size() > 1) {
        this->is_pathing = true;
        if (in_waypoint || this->at_corner) {
            this->at_corner = true;
            this->update_corner_direction();
            Vector next_waypoint = this->path[1].position.clone();
            Vector nearest_point_next_waypoint = next_waypoint.clone();
            nearest_point_next_waypoint.towards(this->position, this->radius);

            Segment seg = {next_waypoint, nearest_point_next_waypoint};
            Segment seg_ccw = rotate_segment(seg, M_PI / 8);
            Segment seg_cw = rotate_segment(seg, -M_PI / 8);

            bool path_clear = this->clear_path_to(seg.p2, walls) ||
                              this->clear_path_to(seg_ccw.p2, walls) ||
                              this->clear_path_to(seg_cw.p2, walls);

            if (path_clear) {
                this->path.pop_front();
                this->immediate_goal = this->path.front().position;
                this->at_corner = false;
            } else {
                this->immediate_goal = this->path.front().position.clone();
                this->immediate_goal.towards(this->position, this->radius);
                Vector increment = this->path.front().position - this->position;
                increment.normalize();
                increment.rotate(this->corner_direction == CLOCKWISE ? M_PI_2 : -M_PI_2);
                increment *= this->radius;
                this->immediate_goal += increment;
            }
        } else {
            this->immediate_goal = this->path.front().position;
        }
    } else {
        this->is_pathing = !near_center;
        this->immediate_goal = this->path.front().position;
    }
}

void Agent::sfm_move(const std::vector<Agent*>& agents, const std::vector<Wall*>& walls, double step_time) {
    this->refresh_immediate_goal(walls);
    Vector acceleration = this->sfm_driving_force(this->immediate_goal) +
                          this->sfm_agent_interaction_force(agents) +
                          this->sfm_wall_interaction_force(walls);

    this->velocity += acceleration * step_time;

    size_t ticks_per_check = 100;

    if (stuck_ticks++ == ticks_per_check) {
        double stuck_factor = 4;
        this->needs_repathing = !this->path.empty() &&
                                this->velocity.norm() < (this->desired_speed / stuck_factor) &&
                                !this->clear_path_to(this->path[0].position, walls);
        this->stuck_ticks = 0;
    }


    if (this->velocity.norm() > this->desired_speed) {
        this->velocity.normalize();
        this->velocity *= this->desired_speed;
    }

    this->position += this->velocity * step_time;

    if (!this->is_pathing) {
        this->stopped_ticks++;
    }
}

Vector Agent::sfm_driving_force(const Vector& position_target) const {
    const double T = 0.54f;    // Relaxation time based on (Moussaid et al., 2009)
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

Vector Agent::sfm_agent_interaction_force(const std::vector<Agent*>& agents) const {
    // Constant Values Based on (Moussaid et al., 2009)
    const double lambda = 2.0;    // Weight reflecting relative importance of velocity vector against position vector
    const double gamma = 0.35f;    // Speed interaction
    const double n_prime = 3.0;    // Angular interaction
    const double n = 2.0;        // Angular intaraction
    const double A = 4.5;        // Modal parameter A

    Vector distance_ij, e_ij, D_ij, t_ij, n_ij, f_ij;
    double B, theta, f_v, f_theta;
    int K;

    for (const Agent* agent_j : agents) {
        // Do Not Compute Interaction Force to Itself
        if (agent_j->id != this->id) {
            // Compute Distance Between Agent j and i
            distance_ij = agent_j->position - this->position;

            // Skip Computation if Agents i and j are Too Far Away
            if (distance_ij.norm() > 2.0) continue;

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

Vector Agent::sfm_wall_interaction_force(const std::vector<Wall*>& walls) const {
    //const double repulsionRange = 0.3f;	// Repulsion range based on (Moussaid et al., 2009)
    const int a = 3;
    const double b = 0.1f;

    Vector nearest_point;
    Vector vector_wi, minVector_wi;
    double square_dist, square_dist_min = INFINITY, d_w, f_iw;

    for (Wall* wall : walls) {
        nearest_point = wall->nearest_point(this->position);
        vector_wi = this->position - nearest_point;    // Vector from wall to agent i
        square_dist = vector_wi.norm_squared();

        // Store Nearest Wall Distance
        if (square_dist < square_dist_min) {
            square_dist_min = square_dist;
            minVector_wi = vector_wi;
        }
    }

    d_w = sqrt(square_dist_min) - this->radius;    // Distance between wall and agent i

    // Compute Interaction Force
    // Formula: f_iw = a * exp(-d_w / b)
    f_iw = a * exp(-d_w / b);
    minVector_wi.normalize();

    return f_iw * minVector_wi;
}
