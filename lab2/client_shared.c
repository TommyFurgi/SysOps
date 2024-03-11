#include <stdio.h>
#include "collatz.h"

int main() {
    printf("Collatz funcion: %d \n", collatz_conjecture(5));
    printf("Test collatz funcion: %d \n", test_collatz_convergence(11, 20));

    return 0;
}