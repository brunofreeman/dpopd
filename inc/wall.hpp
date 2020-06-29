#ifndef __WALL_HPP__
#define __WALL_HPP__

#include "vector.hpp"

struct Line {
 	Vector start;
	Vector end;
};

class Wall {
	public:
		Line wall;
		Wall() : wall((Line){Vector(), Vector()}) {}
		Wall(Vector start, Vector end) : wall((Line){start, end}) {}
		Wall(double x1, double y1, double x2, double y2) : wall((Line){Vector(x1, y1), Vector(x2, y2)}) {}
		Vector nearest_point(Vector position_i);	// Computes distance between 'position_i' and wall
};

#endif // #ifndef __WALL_HPP__