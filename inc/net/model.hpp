#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <string>

class Model {
public:
    std::string environment_name;
    std::string crowd_type;
    double spacial_res;
    double temporal_res;

    Model(std::string  environment_name, std::string crowd_type, const double spacial_res, const double temporal_res)
        : environment_name(std::move(environment_name)), crowd_type(std::move(crowd_type)),
          spacial_res(spacial_res), temporal_res(temporal_res) {}
};


#endif // __MODEL_HPP__
