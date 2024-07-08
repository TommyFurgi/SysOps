#define _XOPEN_SOURCE 700 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

void signal_handler(int signum) {
    printf("Otrzymano SIGUSR1 z numerem: %d\n", signum);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Poprawne użycie: %s <argument>\n", argv[0]);
        return -1;
    }


    if (!strcmp(argv[1], "ignore")) {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1); // wysyla sygnal do samego siebie
    } else if (!strcmp(argv[1], "handler")) {
        signal(SIGUSR1, signal_handler);
        raise(SIGUSR1);
    } else if (!strcmp(argv[1], "mask")) {

        sigset_t sig_set;
        sigemptyset(&sig_set);
        sigaddset(&sig_set, SIGUSR1);

        sigprocmask(SIG_SETMASK, &sig_set, NULL); // ustawia maske dla procesu

        sigset_t pending_signals;
        sigpending(&pending_signals);

        printf("Is signal pending?  %d\n", sigismember(&pending_signals, SIGUSR1));

        raise(SIGUSR1);

        
        sigpending(&pending_signals); // lista sygnałów, które oczekuję na odblokowanie 

        // Sprawdzamy, czy SIGINT jest obecny w zestawie sygnałów oczekujących        
        printf("Is signal pending?  %d\n", sigismember(&pending_signals, SIGUSR1)); // 1 oznacza że SIGUSR1 jest obecny w zestawie sygnałow set, 0 oznacze że nie ma

    } else {
        signal(SIGUSR1, SIG_DFL);
        raise(SIGUSR1); // default exit 
        printf("To nie powino się wyswietlic \n");
    }

    return 0;
}
