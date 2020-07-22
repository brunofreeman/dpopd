#include "sim/wall.hpp"

Vector Wall::nearest_point(Vector position_i) {
    Vector rel_end, rel_pos, rel_end_scale, rel_pos_scale;
    float dotProduct;
    Vector nearestPoint;

    // Create Vector Relative to Wall's 'start'
    rel_end = this->wall.end - this->wall.start;    // Vector from wall's 'start' to 'end'
    rel_pos = position_i - wall.start;    // Vector from wall's 'start' to agent i 'position'

    // Scale Both Vectors by the Length of the Wall
    rel_end_scale = rel_end;
    rel_end_scale.normalize();

    rel_pos_scale = rel_pos * (1.0F / rel_end.norm());

    // Compute Dot Product of Scaled Vectors
    dotProduct = dot(rel_end_scale, rel_pos_scale);

    if (dotProduct < 0.0)        // Position of Agent i located before wall's 'start'
        nearestPoint = wall.start;
    else if (dotProduct > 1.0)    // Position of Agent i located after wall's 'end'
        nearestPoint = wall.end;
    else                        // Position of Agent i located between wall's 'start' and 'end'
        nearestPoint = (rel_end * dotProduct) + wall.start;

    return nearestPoint;
}