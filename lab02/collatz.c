#include <stdio.h>
// #include "collatz.h"

int collatz_conjecture(int value) {
    if ((value % 2) == 0) 
        return (value/2);
    else 
        return (3*value + 1);
} 

int test_collatz_convergence(int value, int max_iter) {
    int counter = 0;

    if (value == 1)
        return 0;

    while (counter < max_iter) {
        value = collatz_conjecture(value);
        counter += 1;

        if (value == 1)
            return counter;
    }   
    return -1;
}
