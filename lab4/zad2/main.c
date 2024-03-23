#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int global_var = 0;

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s <sciezka_do_katalogu> \n", argv[0]);
        return -1;
    }

    printf("Nazwa: %s \n", strrchr(argv[0], '/') + 1);


    int local_var = 0;

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("Błąd: Nie udało się utworzyć nowego procesu potomnego.");

        exit(EXIT_FAILURE);

    } else if (child_pid == 0) { // dziecko
        printf("Proces potomny\n");
        global_var += 1;
        local_var += 1;
        printf("PID dziecka = %d, PID rodzica = %d\n", getpid(), getppid());
        printf("Lokalna zmienna dziecka = %d, globalna zmienna dziecka = %d\n", local_var, global_var);

        int status = execl("/bin/ls", "ls", argv[1], NULL);
        if (status == -1) {
            perror("Błąd exec");
            exit(EXIT_FAILURE);
        }
        exit(status);
        
    }

    // rodzic
    int status = 0;
    wait(&status);
    int child_status = WEXITSTATUS(status);

    printf("Proces rodzica\n");
    printf("PID rodzica = %d, PID dziecka = %d\n", getpid(), child_pid);
    printf("Kod wyjścia dziecka: %d\n", child_status);
    printf("Lokalna zmienna rodzica = %d, globalna zmienna rodzica = %d\n", local_var, global_var);

    return child_status;
}