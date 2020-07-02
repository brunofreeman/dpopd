#ifndef __AGENT_HPP__
#define __AGENT_HPP__

#include <deque>
#include <vector>
#include "wall.hpp"
#include "vector.hpp"
#include "gfx/color.hpp"
#include "polygon.hpp"

struct Waypoint {
	Vector position;
	float radius;
};

class Agent {
    public:
        static int crowd_idx;	// Keep track of 'crowd' vector index in 'SocialForce.h'
        static const size_t shape_sides = 10;

        Polygon* shape;

        int id;
        float radius;
        float desired_speed;
        Color color;

        Vector position;
        std::deque<Waypoint> path;
        Vector velocity;

        Vector driving_force(const Vector position_target);		// Computes f_i
        Vector agent_interaction_force(std::vector<Agent *> agents);	// Computes f_ij
        Vector wall_interaction_force(std::vector<Wall *> walls);		// Computes f_iw

        Agent();
        ~Agent();

        void update_shape();

        void push_waypoint(float x, float y, float radius);

        Vector immediate_goal();
        float orientation() const;
        Vector ahead_vec() const;

        void move(std::vector<Agent *> agents, std::vector<Wall *> walls, float step_time);
};

#endif // #ifndef __AGENT_HPP__