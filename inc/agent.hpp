#ifndef __AGENT_HPP__
#define __AGENT_HPP__

#include "vector.hpp"

class Agent {
    public:
        Vector pos;
        Vector vel;
        Vector acc;
        Vector goal;
        Agent() : pos(), vel(), acc(), goal() {}
        Agent(Vector, Vector, Vector, Vector);
};

#endif  // __AGENT_HPP__