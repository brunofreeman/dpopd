#include "json_wrapper.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include "occupancy_grid.hpp"
#include <bfreeman/dijkstra_polygon.hpp>
#include <iostream>
#include <fstream>

#define MODEL_NAME "first"
#define DATA_OUT_DIR "./dat/" MODEL_NAME "/"
#define FILE_NAME "001"
#define BIN_FILE DATA_OUT_DIR FILE_NAME ".dat"
#define CSV_FILE DATA_OUT_DIR FILE_NAME ".csv"

Model* model;
Environment* environment;
MoveModel* move_model;
OccupancyGrid* occupancy_grid;
MoveModelType mm_type = SOCIAL_FORCE_MODEL;
SpawnGoalType sg_type = RANDOM;
size_t num_agents = 20;
// radii given by Moussaid et al., 2009
float agent_radius = 0.2f;
float waypoint_radius = 3.0f;

unsigned char to_byte(const bool b[8]) {
    unsigned char c = 0;
    for (size_t i = 0; i < 8; ++i) {
        if (b[i]) c |= 1 << i;
    }
    return c;
}

int main(int argc, char** argv) {
    init_random(time(nullptr), time(nullptr), time(nullptr));

    model = json_model(MODEL_NAME);
    environment = json_environment(model->environment_name);
    move_model = new MoveModel(mm_type, sg_type, environment,
                               num_agents, agent_radius, waypoint_radius);
    occupancy_grid = new OccupancyGrid(model, environment);

    size_t update_count = 0;

    occupancy_grid->occupy(move_model->agents);
    // occupancy_grid->print(true);

    std::ofstream bin_fw(BIN_FILE, std::ios::out | std::ios::binary);
    std::ofstream csv_fw;
    csv_fw.open(CSV_FILE);

    if (!bin_fw || !csv_fw) {
        std::cout << "Cannot open file!" << std::endl;
        if (bin_fw) bin_fw.close();
        if (csv_fw) csv_fw.close();
        free(occupancy_grid);
        free(move_model);
        free(model);
        return EXIT_FAILURE;
    }

    csv_fw << "step";

    for (size_t row = 0; row < occupancy_grid->grid.size(); row++) {
        for (size_t col = 0; col < occupancy_grid->grid[row].size(); col++) {
            csv_fw << ',' << row << '.' << col;
        }
    }

    csv_fw << std::endl;

    unsigned long frame = 0;

    unsigned long num_rows = occupancy_grid->grid.size();
    unsigned long num_cols = occupancy_grid->grid[0].size();

    bin_fw.write((char *) &frame, sizeof(frame));
    bin_fw.write((char *) &num_rows, sizeof(num_rows));
    bin_fw.write((char *) &num_cols, sizeof(num_cols));

    while (move_model->pathing()) {
        if (update_count++ == model->updates_per_frame) {
            occupancy_grid->clear_and_occupy(move_model->agents);
            // occupancy_grid->print(true);
            csv_fw << frame;
            for (auto& row : occupancy_grid->grid) {
                for (auto&& cell : row) {
                    csv_fw << ',' << (cell ? 1 : 0);
                }
            }
            csv_fw << std::endl;

            size_t bool_idx = 0;
            bool bools[8];
            for (bool& i : bools) i = false;

            for (auto& row : occupancy_grid->grid) {
                for (auto&& cell : row) {
                    bools[bool_idx++] = cell;
                    if (bool_idx == 8) {
                        unsigned char bool_pack = to_byte(bools);
                        bin_fw.write((char *) &bool_pack, sizeof(bool_pack));
                        bool_idx = 0;
                        for (bool& i : bools) i = false;
                    }
                }
            }

            if (bool_idx != 0) {
                unsigned char bool_pack = to_byte(bools);
                bin_fw.write((char *) &bool_pack, sizeof(bool_pack));
            }

            frame++;
            update_count = 0;
        }
        // csv_fw.write((char *) occupancy_grid, sizeof(OccupancyGrid));
        move_model->move_crowd(model->update_res);
    }

    bin_fw.seekp(std::ios::beg);
    bin_fw.write((char *) &frame, sizeof(frame));

    csv_fw.close();
    bin_fw.close();
    free(occupancy_grid);
    free(move_model);
    free(model);

    return EXIT_SUCCESS;
}
