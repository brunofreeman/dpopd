#include "json_wrapper.hpp"
#include "move_model.hpp"
#include "random_wrapper.hpp"
#include <bfreeman/dijkstra_polygon.hpp>
#include <iostream>

const std::string model_name = "first";
Model* model;
Environment* environment;
MoveModel* move_model;
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

    size_t update_count = 0;

    while (move_model->pathing()) {
        if (update_count == 0) {
            // record a frame
        }

        move_model->move_crowd(model->update_res);

        update_count++;
        update_count %= model->updates_per_frame;
    }

    free(model);
    free(move_model);
}
