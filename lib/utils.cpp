#include "utils.h"

#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()
#include <random>

int randint(int lower_bound, int upper_bound, 
    std::mt19937& rng)
{
    std::uniform_int_distribution<int> unif(lower_bound, upper_bound);

    return unif(rng);
    // return (rand() % (max - min + 1)) + min;
}

int randint_diff(int min, int max, int avoid, 
    std::mt19937& rng)
{
    int num;
    do
        num = randint(min, max, rng);
    while (num == avoid);
    return num;
}

double randdouble(double lower_bound, double upper_bound, 
    std::mt19937& rng)
{
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    
    double a_random_double = unif(rng);

    return a_random_double;
}