#ifndef __MOVE_MODEL_HPP__
#define __MOVE_MODEL_HPP__

#include <vector>
#include "agent.hpp"
#include "wall.hpp"

/*
 * Tracks a crowd of agents as well as walls that define
 * the environment. Currently, the Social Force Model is
 * the only implemented functionality.
 */
class MoveModel {
public:
    std::vector<Agent*> crowd;
    std::vector<Wall*> walls;

    ~MoveModel();

    void add_agent(Agent* agent);

    void add_wall(Wall* wall);

    void delete_agent();

    void delete_crowd();

    void delete_walls();

    void sfm_move_crowd(float step_time);
};

#endif // #ifndef __MOVE_MODEL_HPP__