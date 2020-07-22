#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "vector.hpp"

/*
 * Defines the orientation of the points
 * defined by three vectors
 */
enum Orientation {
    COUNTERCLOCKWISE = -1,
    COLINEAR = 0,
    CLOCKWISE = 1
};

struct Segment {
    Vector p1;
    Vector p2;
};

// assumes that point lies on the line parallel to seg
bool on_segment(const Vector& point, const Segment& seg);

// returns the direction of motion traversing p->q->r
Orientation orientation(const Vector& p, const Vector& q, const Vector& r);

bool check_intersect(const Segment& seg1, const Segment& seg2);

bool check_intersect_no_endpoints(const Segment& seg1, const Segment& seg2);

double length(const Segment&);

// returns the shortest distance between point and seg
double distance(const Vector& point, const Segment& seg);

// returns a Segment that is seg rotated around its p1
Segment rotate_segment(const Segment& seg, double angle);

#endif // __SEGMENT_H__
