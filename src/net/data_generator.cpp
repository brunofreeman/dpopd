#include "json_wrapper.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include "occupancy_grid.hpp"
#include <bfreeman/dijkstra_polygon.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#define MODEL_NAME "first"
#define DATA_OUT_DIR "./dat/" MODEL_NAME "/"
#define BIN_EXT ".dat"
#define CSV_EXT ".csv"
#define FILE_DIGITS 4

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

int usage() {
    std::cerr << "usage: ./data_generator [test/train dir] [starting test file index] [# test files to generate]" << std::endl;
    return EXIT_FAILURE;
}

std::string zero_pad(size_t num, size_t width) {
    std::ostringstream ss;
    ss << std::setw(width) << std::setfill('0') << num;
    return ss.str();
}

void print_warning() {
    std::cout << "WARNING: Generation of a file may run indefinitely"          << std::endl;
    std::cout << "         if two agents are given similar goal destinations." << std::endl;
    std::cout << "         Abort execution and delete the most recent file"    << std::endl;
    std::cout << "         in this case."                                      << std::endl << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        return usage();
    }

    size_t file_idx;
    size_t num_to_gen;

    try {
        file_idx = std::stoi(argv[2]);
        num_to_gen = std::stoi(argv[3]);
    } catch (const std::invalid_argument& ex) {
        return usage();
    } catch (const std::out_of_range& ex) {
        return usage();
    }

    print_warning();

    init_random(time(nullptr), time(nullptr), time(nullptr));

    for (size_t i = 0; i < num_to_gen; i++) {
        model = json_model(MODEL_NAME);
        environment = json_environment(model->environment_name);
        move_model = new MoveModel(mm_type, sg_type, environment,
                                   num_agents, agent_radius, waypoint_radius);
        occupancy_grid = new OccupancyGrid(model, environment);

        size_t update_count = 0;

        occupancy_grid->occupy(move_model->agents);
        // occupancy_grid->print(true);
        std::string file_name = zero_pad(file_idx++, FILE_DIGITS);
        std::string bin_file = std::string(DATA_OUT_DIR) + argv[1] + '/' + file_name + BIN_EXT;
        std::ofstream bin_fw(bin_file, std::ios::out | std::ios::binary);
        // std::ofstream csv_fw;
        // csv_fw.open(CSV_FILE);

        if (!bin_fw /*|| !csv_fw*/) {
            std::cout << "Cannot open file!" << std::endl;
            if (bin_fw) bin_fw.close();
            // if (csv_fw) csv_fw.close();
            free(occupancy_grid);
            free(move_model);
            free(model);
            return EXIT_FAILURE;
        }

        // csv_fw << "step";

        /*for (size_t row = 0; row < occupancy_grid->grid.size(); row++) {
            for (size_t col = 0; col < occupancy_grid->grid[row].size(); col++) {
                csv_fw << ',' << row << '.' << col;
            }
        }

        csv_fw << std::endl;*/

        unsigned long frame = 0;

        unsigned long num_rows = occupancy_grid->grid.size();
        unsigned long num_cols = occupancy_grid->grid[0].size();

        bin_fw.write((char*) &frame, sizeof(frame));
        bin_fw.write((char*) &num_rows, sizeof(num_rows));
        bin_fw.write((char*) &num_cols, sizeof(num_cols));

        std::cout << "Writing file " << file_name << ", frame 0";

        while (move_model->pathing()) {
            if (update_count++ == model->updates_per_frame) {
                occupancy_grid->clear_and_occupy(move_model->agents);
                // occupancy_grid->print(true);
                /*csv_fw << frame;
                for (auto& row : occupancy_grid->grid) {
                    for (auto&& cell : row) {
                        csv_fw << ',' << (cell ? 1 : 0);
                    }
                }
                csv_fw << std::endl;*/

                size_t bool_idx = 0;
                bool bools[8];
                for (bool& i : bools) i = false;

                for (auto& row : occupancy_grid->grid) {
                    for (auto&& cell : row) {
                        bools[bool_idx++] = cell;
                        if (bool_idx == 8) {
                            unsigned char bool_pack = to_byte(bools);
                            bin_fw.write((char*) &bool_pack, sizeof(bool_pack));
                            bool_idx = 0;
                            for (bool& i : bools) i = false;
                        }
                    }
                }

                if (bool_idx != 0) {
                    unsigned char bool_pack = to_byte(bools);
                    bin_fw.write((char*) &bool_pack, sizeof(bool_pack));
                }

                frame++;
                update_count = 0;

                std::cout << "\rWriting file " << file_name << ", frame " << frame;
            }
            // csv_fw.write((char *) occupancy_grid, sizeof(OccupancyGrid));
            move_model->move_crowd(model->update_res);
        }

        std::cout << std::endl;

        bin_fw.seekp(std::ios::beg);
        bin_fw.write((char*) &frame, sizeof(frame));

        // csv_fw.close();
        bin_fw.close();
        free(occupancy_grid);
        free(move_model);
        free(model);
    }

    return EXIT_SUCCESS;
}
