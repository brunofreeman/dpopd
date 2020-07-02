#include <math.h>
#include "polygon.hpp"

Polygon::Polygon(Vector* vertices, const size_t vertices_s) :
    vertices(vertices), vertices_s(vertices_s) {}

Polygon::~Polygon() {
    delete[] this->vertices;
}

bool Polygon::is_interior_point(const Vector& point) const {
    double max_x = 0;
    for (size_t i = 0; i < this->vertices_s; i++) {
        if (this->vertices[i].x > max_x) max_x = this->vertices[i].x;
    }

	Segment pos_x_ray = {point, Vector(max_x + 1, point.y)};
	size_t intersections = 0;
    size_t curr_idx = 0;

	do {
		size_t next_idx = (curr_idx + 1) % this->vertices_s;
		if (check_intersect((Segment){this->vertices[curr_idx],
                                      this->vertices[next_idx]}, pos_x_ray)) {
			intersections++;
		}
		curr_idx = next_idx;
	} while (curr_idx != 0);

	return intersections % 2 == 1;
}

std::string Polygon::to_string() const {
    std::string poly_str = "[";

    for (size_t i = 0; i < this->vertices_s; i++){
        poly_str += this->vertices[i].to_string();
        if (i != this->vertices_s - 1) {
            poly_str += ", ";
        }
    }

    poly_str += "]";
    
    return poly_str;
}

Polygon* regular_ngon(const Vector& center, double r, size_t n) {
	Vector* vertices = new Vector[n];
    for (size_t i = 0; i < n; i++) {
        Vector vertex(0, r);
        vertex.rotate(i * (2 * M_PI) / n);
        vertex += center;
        vertices[i] = vertex;
    }
    return new Polygon(vertices, n);
}