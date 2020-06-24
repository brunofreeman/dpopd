#ifndef __JSON_WRAPPER_HPP__
#define __JSON_WRAPPER_HPP__

#include "environment.hpp"

#define ENV_PATH "./def/env/"
#define MDL_PATH "./def/mdl/"
#define JSON_ID_BORDER "border"
#define JSON_ID_OBSTACLES "obstacles"

Environment *json_environment(char *filename);

#endif // #ifndef __JSON_WRAPPER_HPP__