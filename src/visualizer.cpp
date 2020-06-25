#include <iostream>
#include "json_wrapper.hpp"

int main(int argc, char **argv) {
    Environment *environment = json_environment("square");

    std::cout << (*environment).to_string();

    delete environment;
    return 0;
}