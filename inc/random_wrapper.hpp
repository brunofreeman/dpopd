#ifndef __RANDOM_WRAPPER_HPP__
#define __RANDOM_WRAPPER_HPP__

void init_random(long seed1, long seed2, long seed3);

double from_normal_distribution(double mean, double std_dev);

size_t strand(size_t min, size_t max);

#endif //__RANDOM_WRAPPER_HPP__
