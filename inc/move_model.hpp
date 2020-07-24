#ifndef __MOVE_MODEL_HPP__
#define __MOVE_MODEL_HPP__

#include <vector>
#include "agent.hpp"
#include "wall.hpp"
#include "environment.hpp"
#include <bfreeman/dijkstra_polygon.hpp>

enum SpawnGoalType {
    RANDOM = 0
};

enum MoveModelType {
    SOCIAL_FORCE_MODEL = 0
};

/*
 * Tracks a crowd of agents as well as walls that define
 * the environment. Currently, the Social Force Model is
 * the only implemented functionality.
 */
class MoveModel {
public:
    std::vector<Agent*> agents;
    std::vector<Wall*> walls;
    std::vector<std::vector<bfreeman::Point>> dijkstra_polygon;
    MoveModelType type;
    double agent_radius;
    double waypoint_radius;
    Environment* environment;

    MoveModel(MoveModelType type, SpawnGoalType spawn_goal_type,
            Environment* environment, size_t num_agents,
              double agent_radius, double waypoint_radius);

    ~MoveModel();

    bool pathing();

    void create_agents(size_t num_agents);
    void set_agent_waypoints(Agent* agent, const Vector& goal) const;
    void populate_dijkstra_polygon();
    void add_polygon_walls(const Polygon* polygon);
    void create_walls();

    void add_agent(Agent* agent);

    void add_wall(Wall* wall);

    void delete_agent();

    void delete_crowd();

    void delete_walls();

    void move_crowd(double step_time);
    void sfm_move_crowd(double step_time);
};

#endif // #ifndef __MOVE_MODEL_HPP__