#include <iostream>
#include <string>
#include "json_wrapper.hpp"

const std::string model_name = "first";

int main(int argc, char** argv) {
    Model* model = json_model(model_name);
    std::cout << model->environment_name << ", " << model->crowd_type <<
                ", " << model->spacial_res << ", " << model->temporal_res << std::endl;
}
