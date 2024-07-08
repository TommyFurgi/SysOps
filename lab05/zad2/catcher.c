#define _XOPEN_SOURCE 700 

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int status = -1;
int status_changes = 0;

void update_status(int argument){
    status_changes++;
    status = argument;
}

void signal_action(int signo, int sender_pid) {
    printf("Otrzymano status: %d od procesu: %d\n", status, sender_pid);

    if (sender_pid != -1) // wysyla sygnal do sendera
        kill(sender_pid, SIGUSR1);
}

void sigusr_handler(int signo, siginfo_t *info, void *context) {
    int int_val = info->si_int;

    update_status(int_val);
    
    signal_action(signo, info->si_pid);
}

int main() {
    printf("Catcher - numer PID: %d \n", getpid());

    struct sigaction action; // struktura przechowywujaca info dot obslugi sygnalow
    action.sa_handler = sigusr_handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigaction(SIGUSR1, &action, NULL); // rejestracja funcji obslugi sygnalow

    while(1) {
        switch(status) {
            case 1:
                for(int i = 1; i <= 100; i++)
                    printf("%i, ", i);
            
                printf("\n");
                status = -1;
                break;
            case 2:
                printf("Liczba zmian statusu %d\n", status_changes);
                status = -1;
                break;
            case 3:
                printf("Koniec programu \n");
                exit(0);
                break;
            default:
                break; 
        }
    }

    return 0;
}