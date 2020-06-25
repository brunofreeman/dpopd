#include <iostream>
#include "json_wrapper.hpp"

int main(int argc, char **argv) {
    Environment *environment = json_environment("two_sqr_in_sqr");

    std::cout << (*environment).to_string() << "\n";

    delete environment;
    return 0;
}