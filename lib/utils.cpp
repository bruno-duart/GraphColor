#include "utils.h"

#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()
#include <random>

int randint(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

int randint_diff(int min, int max, int avoid)
{
    int num;
    do
        num = randint(min, max);
    while (num == avoid);
    return num;
}

double randdouble(double lower_bound, double upper_bound)
{
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    std::default_random_engine re;
    double a_random_double = unif(re);

    return a_random_double;
}