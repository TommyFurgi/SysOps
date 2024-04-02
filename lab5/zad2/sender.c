#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h> 


void signal_handler(int signal){
    printf("Potwierdzenie otrzymania \n");
}

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Poprawne użycie: %s <pid> <argument> \n", argv[0]);
        return -1;
    }   

    printf("Sender PID: %d\n", getpid());


    signal(SIGUSR1, signal_handler); // wywoluje funcje po otrzymaniu sygnału

    int catcher_pid = strtol(argv[1], NULL, 10);
    int catcher_argument  = strtol(argv[2], NULL, 10);

    if (kill(catcher_pid, 0) != 0) { // sprawdza czy isnieje taki proces
        printf("Proces o PID %d nie istnieje.\n", catcher_pid);
        return -1;
    }
    

    union sigval value = {catcher_argument};
    sigqueue(catcher_pid, SIGUSR1, value); // wysyła sygnał do podanego procesu 

    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    sigsuspend(&mask); // zawiesza proces, czekając na sygnał ze zbioru mask - kiedy sygnał zostanie przechwycony, proces zostanie wznowiony
    
    return 0;
}