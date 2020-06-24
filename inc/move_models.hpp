#ifndef __MOVE_MODELS_HPP__
#define __MOVE_MODELS_HPP__

#include <cstdlib>
#include "agent.hpp"

struct InfoSFM {
    // environment
    // obstacles
};

void update_sfm(Agent **agents, size_t agents_s, InfoSFM *info);

#endif // #ifndef __MOVE_MODELS_HPP__