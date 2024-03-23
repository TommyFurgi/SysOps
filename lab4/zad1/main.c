#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2){
        fprintf(stderr, "Sposób użycia: %s <ilosc_procesów> \n", argv[0]);
        return -1;
    }

    long long processes = strtol(argv[1], NULL, 10);

    for (int i=0; i<processes; i++){

        pid_t child_pid;
        child_pid = fork(); // tworzy nowy proces

        if (child_pid == 0) {
            printf("Obecny pid: %d, pid rodzica: %d \n", (int)getpid(), (int)getppid());
            
            exit(0);
        }
    }

    while(wait(NULL) > 0); // oczekuje na zakończenie wszytkich procesów potomnych

    printf("Liczba procesów jakie zostały utworzone: %lld \n", processes);

    return 0;
}