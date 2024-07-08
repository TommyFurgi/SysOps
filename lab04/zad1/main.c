#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2){
        fprintf(stderr, "Sposób użycia: %s <ilosc_procesów> \n", argv[0]);
        return -1;
    }

    long long processes = strtol(argv[1], NULL, 10); // zamiana stringa na inta

    for (int i=0; i<processes; i++){

        pid_t child_pid;
        child_pid = fork(); // tworzy nowy proces
        // if child_pid == 0 to proces potomka
        // if child_pid == pid potomka to proces rodzica
        // if child_pid < 0 błąd

        if (child_pid == 0) {
            printf("Obecny pid: %d, pid rodzica: %d \n", (int)getpid(), (int)getppid()); // pid_t getpid(void);
            
            exit(0);
        }
    }

    while(wait(NULL) > 0); // oczekuje na zakończenie wszytkich procesów potomnych
    // gdy nie ma procesów potomnych wait(NULL) zwraca -1 

    printf("Liczba procesów jakie zostały utworzone: %lld \n", processes);

    return 0;
}