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

    bool is_pathing;
    bool at_corner;
    Orientation corner_direction;
    //Vector last_waypoint_pos;
    Vector immediate_goal;

    Vector prev_update_pos;
    Polygon* shape;

    size_t id;
    float radius;
    float desired_speed;

    Vector position;
    std::deque<Waypoint> path;
    Vector velocity;

    explicit Agent(const MoveModelType& move_model_type, float radius);

    ~Agent();

    void update_shape();

    void push_waypoint(float x, float y, float waypoint_radius);

    [[nodiscard]] bool clear_path_to(const Vector& to, const std::vector<Wall*>& walls) const;

    void update_corner_direction();

    //Wall* get_nearest_corner_wall(const std::vector<Wall*>& walls) const;

    void refresh_immediate_goal(const std::vector<Wall*>& walls);

    [[nodiscard]] Vector sfm_driving_force(const Vector& position_target) const;
    [[nodiscard]] Vector sfm_agent_interaction_force(const std::vector<Agent*>& agents) const;
    [[nodiscard]] Vector sfm_wall_interaction_force(const std::vector<Wall*>& walls) const;
    void sfm_move(const std::vector<Agent*>& agents, const std::vector<Wall*>& walls, float step_time);
};

#endif // #ifndef __AGENT_HPP__