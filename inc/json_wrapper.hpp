#ifndef __JSON_WRAPPER_HPP__
#define __JSON_WRAPPER_HPP__

#include "environment.hpp"
#include "model.hpp"

#define ENV_PATH "./def/env/"
#define ENV_ID_BORDER "border"
#define ENV_ID_OBSTACLES "obstacles"
#define ENV_ID_SCALE "scale"

#define MDL_PATH "./def/mdl/"
#define MDL_ID_ENVIRONMENT "environment"
#define MDL_ID_CROWD_TYPE "crowd_type"
#define MDL_ID_SPACIAL_RES "spacial_res"
#define MDL_ID_TEMPORAL_RES "temporal_res"
#define MDL_ID_UPDATE_RES "update_res"
#define MDL_ID_UPDATES_PER_FRAME "updates_per_frame"

// constructs an Environment object from its JSON definition
Environment* json_environment(const std::string& filename);

Model* json_model(const std::string& filename);

#endif // #ifndef __JSON_WRAPPER_HPP__