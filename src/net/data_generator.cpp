#include "json_wrapper.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include "occupancy_grid.hpp"
#include <bfreeman/dijkstra_polygon.hpp>
#include <iostream>

const std::string model_name = "first";
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

int main(int argc, char** argv) {
    init_random(time(nullptr), time(nullptr), time(nullptr));

    model = json_model(model_name);
    environment = json_environment(model->environment_name);
    move_model = new MoveModel(mm_type, sg_type, environment,
                               num_agents, agent_radius, waypoint_radius);
    occupancy_grid = new OccupancyGrid(model, environment);

    size_t update_count = 0;

    occupancy_grid->occupy(move_model->agents);
    occupancy_grid->print(true);

    while (move_model->pathing()) {
        if (update_count++ == model->updates_per_frame) {
            occupancy_grid->clear_and_occupy(move_model->agents);
            occupancy_grid->print(true);
            update_count = 0;
        }

        move_model->move_crowd(model->update_res);
    }

    free(occupancy_grid);
    free(move_model);
    free(model);
}
