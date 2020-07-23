#include "occupancy_grid.hpp"

OccupancyGrid::OccupancyGrid(Model* model, Environment *environment) {

}

void OccupancyGrid::clear() {
    for (auto& col : this->grid) {
        for (auto&& cell : col) {
            cell = false;
        }
    }
}

bool OccupancyGrid::are_valid_coords(size_t row, size_t col) {
    return row < this->grid.size() && col < this->grid[0].size();
}

bool OccupancyGrid::occupy_if_valid(size_t row, size_t col) {
    bool valid = this->are_valid_coords(row, col);
    if (valid) this->grid[row][col] = true;
    return valid;
}

void OccupancyGrid::occupy(const Vector& position, const double radius) {
    size_t col = position.x / this->cell_size;
    size_t row = position.y / this->cell_size;
    this->grid[row][col] = true;

    size_t max_layer_away = (radius / this->cell_size) + 1;

    for (size_t i = 0; i < max_layer_away; i++) {
        // check orthogonals
        Vector above(position.x, (row + 1 + i) * this->cell_size);
        Vector below(position.x, (row     - i) * this->cell_size);
        Vector  left((col     - i) * this->cell_size, position.y);
        Vector right((col + 1 + i) * this->cell_size, position.y);

        if (distance(position, above) <= radius) {
            this->occupy_if_valid(row + 1 + i, col);
        }
        if (distance(position, below) <= radius) {
            this->occupy_if_valid(row - 1 - i, col);
        }
        if (distance(position, left) <= radius) {
            this->occupy_if_valid(row, col - 1 - i);
        }
        if (distance(position, right) <= radius) {
            this->occupy_if_valid(row, col + 1 + i);
        }

        // check diagonals
        Vector     top_left((col     - i) * this->cell_size, (row + 1 + i) * this->cell_size);
        Vector    top_right((col + 1 + i) * this->cell_size, (row + 1 + i) * this->cell_size);
        Vector  bottom_left((col     - i) * this->cell_size, (row     - i) * this->cell_size);
        Vector bottom_right((col + 1 + i) * this->cell_size, (row     - i) * this->cell_size);

        if (distance(position, top_left) <= radius) {
            this->occupy_if_valid(row + 1 + i, col - 1 - i);
        }
        if (distance(position, top_right) <= radius) {
            this->occupy_if_valid(row + 1 + i, col + 1 + i);
        }
        if (distance(position, bottom_left) <= radius) {
            this->occupy_if_valid(row - 1 - i, col - 1 - i);
        }
        if (distance(position, bottom_right) <= radius) {
            this->occupy_if_valid(row - 1 - i, col + 1 + i);
        }

        // check above skewed
        for (size_t j = 0; j < i; i++) {
            Vector  above_left((col - i + j + 1) * this->cell_size,(row + 1 + i) * this->cell_size);
            Vector above_right((col + i - j) * this->cell_size,    (row + 1 + i) * this->cell_size);
            if (distance(position, above_left) <= radius) {
                this->occupy_if_valid(row + 1 + i, col - i + j);
            }
            if (distance(position, above_right) <= radius) {
                this->occupy_if_valid(row + 1 + i, col + i - j);
            }
        }
        // check below skewed
        for (size_t j = 0; j < i; i++) {
            Vector  below_left((col - i + j + 1) * this->cell_size,(row - i) * this->cell_size);
            Vector below_right((col + i - j) * this->cell_size,    (row - i) * this->cell_size);
            if (distance(position, below_left) <= radius) {
                this->occupy_if_valid(row - 1 - i, col - i + j);
            }
            if (distance(position, below_right) <= radius) {
                this->occupy_if_valid(row - 1 - i, col + i - j);
            }
        }
        // check left skewed
        for (size_t j = 0; j < i; i++) {
            Vector left_above((col - i) * this->cell_size,(row + i - j) * this->cell_size);
            Vector left_below((col - i) * this->cell_size,(row - i + 1 + j) * this->cell_size);
            if (distance(position, left_above) <= radius) {
                this->occupy_if_valid(row + i - j, col - 1 - i);
            }
            if (distance(position, left_below) <= radius) {
                this->occupy_if_valid(row - i + j, col - 1 - i);
            }
        }
        // check right skewed
        for (size_t j = 0; j < i; i++) {
            Vector right_above((col + 1 + i) * this->cell_size,(row + i - j) * this->cell_size);
            Vector right_below((col + 1 + i) * this->cell_size,(row - i + 1 + j) * this->cell_size);
            if (distance(position, right_above) <= radius) {
                this->occupy_if_valid(row + i - j, col + 1 + i);
            }
            if (distance(position, right_below) <= radius) {
                this->occupy_if_valid(row - i + j, col + 1 + i);
            }
        }
    }
}

void OccupancyGrid::occupy(const std::vector<Agent*>& agents) {
    for (auto& agent : agents) {
        this->occupy(agent->position, agent->radius);
    }
}

void OccupancyGrid::clear_and_occupy(const std::vector<Agent*>& agents) {
    this->clear();
    this->occupy(agents);
}
