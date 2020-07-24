#ifndef __OCCUPANCY_GRID_HPP__
#define __OCCUPANCY_GRID_HPP__

#include <vector>
#include "model.hpp"
#include "environment.hpp"
#include "agent.hpp"

class OccupancyGrid {
public:
    static const char    empty_cell_char = ' ';
    static const char occupied_cell_char = '*';

    std::vector<std::vector<bool>> grid;
    double cell_size;

    OccupancyGrid(Model* model, Environment* environment);

    void clear();
    void occupy(const Vector& position, const double radius);
    void occupy(const std::vector<Agent*>& agents);
    void clear_and_occupy(const std::vector<Agent*>& agents);
    bool are_valid_coords(size_t row, size_t col);
    bool  occupy_if_valid(size_t row, size_t col);

    void print(const bool show_grid_border);
};

#endif // __OCCUPANCY_GRID_HPP__
