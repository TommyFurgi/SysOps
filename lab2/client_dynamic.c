#include <stdio.h>
#include <dlfcn.h>

#ifndef MAX_ITER
#define MAX_ITER 100 // Domyślna wartość maksymalnej liczby iteracji

#endif

int main() {
    void *handler = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handler) {
        printf("blad otw.biblioteki\n");
        return 0;
    }

    int (*collatz_conjecture)(int input);
    int (*test_collatz_convergence)(int input, int max_iter);

    collatz_conjecture = dlsym(handler, "collatz_conjecture");
    test_collatz_convergence = dlsym(handler, "test_collatz_convergence");
    if (dlerror() != 0) {
        printf("blad dlsym\n");
        return 0;
    }

    int max_iter = MAX_ITER;
    printf("Collatz funcion: %d \n", collatz_conjecture(5));
    printf("Test collatz funcion: %d \n", test_collatz_convergence(11,max_iter));
 
    dlclose(handler);
    return 0;
}