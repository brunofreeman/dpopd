#ifndef __SOCIAL_FORCE_HPP__
#define __SOCIAL_FORCE_HPP__

#include <vector>
#include "agent.hpp"
#include "wall.hpp"

class SocialForce {
	public:
		std::vector<Agent *> crowd;
		std::vector<Wall *> walls;

		~SocialForce();

		void add_agent(Agent *agent);
		void add_wall(Wall *wall);

		void delete_agent(); // Removes individual or single group
		void delete_crowd(); // Remove all individuals and groups
		void delete_walls();
		void move_crowd(float step_time);
};

#endif // #ifndef __SOCIAL_FORCE_HPP__