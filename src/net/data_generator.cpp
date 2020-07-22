#include <chrono>
#include "random_wrapper.hpp"

int main(int argc, char** argv) {
    init_random(time(nullptr), time(nullptr), time(nullptr));
}
