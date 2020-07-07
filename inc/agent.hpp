#ifndef __AGENT_HPP__
#define __AGENT_HPP__

#include <deque>
#include <vector>
#include "wall.hpp"
#include "vector.hpp"
#include "gfx/color.hpp"
#include "polygon.hpp"

enum MoveModelType {
    SOCIAL_FORCE_MODEL = 0
};

struct Waypoint {
	Vector position;
	float radius;
};

class Agent {
    public:
        static int crowd_idx;
        static const size_t shape_sides = 10;

        Vector prev_update_pos;
        Polygon* shape;

        int id;
        float radius;
        float desired_speed;
        Color color;

        Vector position;
        std::deque<Waypoint> path;
        Vector velocity;

        Agent(const MoveModelType& move_model_type);
        ~Agent();

        void update_shape();

        void push_waypoint(float x, float y, float radius);

        Vector immediate_goal();
        float orientation() const;
        Vector ahead_vec() const;

        Vector sfm_driving_force(const Vector position_target);		    // Computes f_i
        Vector sfm_agent_interaction_force(std::vector<Agent*> agents);	// Computes f_ij
        Vector sfm_wall_interaction_force(std::vector<Wall*> walls);    // Computes f_iw
        void sfm_move(std::vector<Agent*> agents, std::vector<Wall*> walls, float step_time);
};

#endif // #ifndef __AGENT_HPP__