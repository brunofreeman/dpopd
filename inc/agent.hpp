#ifndef __AGENT_HPP__
#define __AGENT_HPP__

#include <deque>
#include <vector>
#include "wall.hpp"
#include "vector.hpp"
#include "polygon.hpp"

enum MoveModelType {
    SOCIAL_FORCE_MODEL = 0
};

struct Waypoint {
    Vector position;
    float radius {0.0f};
};

class Agent {
public:
    static size_t crowd_size;
    static const size_t shape_sides = 10;

    Vector prev_update_pos;
    Polygon* shape;

    size_t id;
    float radius;
    float desired_speed;

    Vector position;
    std::deque<Waypoint> path;
    Vector velocity;

    explicit Agent(const MoveModelType& move_model_type);

    ~Agent();

    void update_shape();

    void push_waypoint(float x, float y, float waypoint_radius);

    Vector immediate_goal();

    [[nodiscard]] Vector sfm_driving_force(const Vector& position_target) const;            // Computes f_i
    [[nodiscard]] Vector sfm_agent_interaction_force(const std::vector<Agent*>& agents) const;    // Computes f_ij
    [[nodiscard]] Vector sfm_wall_interaction_force(const std::vector<Wall*>& walls) const;    // Computes f_iw
    void sfm_move(const std::vector<Agent*>& agents, const std::vector<Wall*>& walls, float step_time);
};

#endif // #ifndef __AGENT_HPP__