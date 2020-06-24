#ifndef __MOVE_MODELS_HPP__
#define __MOVE_MODELS_HPP__

#include <cstdlib>
#include "agent.hpp"

struct InfoUnchanged {};

struct InfoSFM {
    // environment
    // obstacles
};

void update_unchanged(Agent **agents, size_t num_agents, InfoUnchanged *info);

void update_sfm(Agent **agents, size_t num_agents, InfoSFM *info);

#endif // #ifndef __MOVE_MODELS_HPP__