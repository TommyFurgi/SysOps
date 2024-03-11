#include <stdio.h>


#ifdef DYNAMIC_CLIENT
    #include "dlfcn.h"

    int (*collatz_conjecture)(int input);
    int (*test_collatz_convergence)(int input, int max_iter);
#else
    #include "collatz.h"
#endif

int main() {
    #ifdef DYNAMIC_CLIENT
        void *handler = dlopen("./libcollatz.so", RTLD_LAZY);
        if (!handler) {
            printf("blad otw.biblioteki\n");
            return 0;
        }

        // int (*collatz_conjecture)(int input);
        // int (*test_collatz_convergence)(int input, int max_iter);

        collatz_conjecture = dlsym(handler, "collatz_conjecture");
        test_collatz_convergence = dlsym(handler, "test_collatz_convergence");
        if (dlerror() != 0) {
            printf("blad dlsym\n");
            return 0;
        }
        printf("ala ma kota");

    #endif
    printf("Collatz funcion: %d \n", collatz_conjecture(5));
    printf("Test collatz funcion: %d \n", test_collatz_convergence(11,20));
    
    #ifdef DYNAMIC_CLIENT
        dlclose(handler);
    #endif
    return 0;
}