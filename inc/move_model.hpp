#ifndef __MOVE_MODEL_HPP__
#define __MOVE_MODEL_HPP__

#include <vector>
#include "agent.hpp"
#include "wall.hpp"

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