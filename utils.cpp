#include "utils.h"

#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()

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
