#include <cmath>
#include "sim/polygon.hpp"
#include "sim/segment.hpp"

bool Polygon::is_interior_point(const Vector& point, double radius) const {
    double max_x = 0;
    for (auto vertex : this->vertices) {
        if (vertex.x > max_x) max_x = vertex.x;
    }

    Segment pos_x_ray = {point, Vector(max_x + 1, point.y)};
    size_t intersections = 0;
    size_t curr_idx = 0;

    do {
        size_t next_idx = (curr_idx + 1) % this->vertices.size();
        Segment seg = {this->vertices[curr_idx], this->vertices[next_idx]};
        if (distance(point, seg) < radius) return false;
        if (check_intersect(seg, pos_x_ray)) intersections++;
        curr_idx = next_idx;
    } while (curr_idx != 0);

    return intersections % 2 == 1;
}

bool Polygon::is_interior_point(const Vector& point) const {
    return this->is_interior_point(point, 0.0f);
}

Polygon* regular_ngon(const Vector& center, double r, size_t n) {
    std::vector<Vector> vertices(n);
    for (size_t i = 0; i < n; i++) {
        Vector vertex(0, r);
        vertex.rotate(i * (2 * M_PI) / n);
        vertex += center;
        vertices[i] = vertex;
    }
    return new Polygon(vertices);
}