#include <random>
#include "random_wrapper.hpp"

static std::default_random_engine engine;

void init_random(const long seed1, const long seed2, const long seed3) {
    srand(seed1);
    srand48(seed2);
    engine.seed(seed3);
}

double from_normal_distribution(const double mean, const double std_dev) {
    std::normal_distribution<double> distribution(mean, std_dev);
    return distribution(engine);
}

size_t strand(size_t min, size_t max) {
    size_t range = max - min + 1;
    return (rand() % range) + min;
}