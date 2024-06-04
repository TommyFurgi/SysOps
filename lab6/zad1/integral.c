#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

double function(double x) { // 3.1416
    return 4 / (x * x + 1);
}

double calculate_integral(double start, double end, double width) {
    double sum = 0;
    double x;
    for (x = start; x < end; x += width) {
        sum += function(x) * width;
    }
    return sum;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        fprintf(stderr, "Poprawne użycie: %s <szerokość_prostokąta> <liczba_procesów> \n", argv[0]);
        return -1;
    }

    double width = strtod(argv[1], NULL);
    long num_processes = strtol(argv[2], NULL, 10);

    double start = 0.0;
    double end = 1.0;
    double total_integral = 0;
    int pipes_fd[num_processes][2];

    if ((double)1/num_processes < width) {
        fprintf(stderr, "Bład, za mała dokładność dla tej takiej liczby procesów \n");
        return -1;
    }


    for (int i = 0; i < num_processes; i++){
        pid_t pid;

        if (pipe(pipes_fd[i]) == -1) { // w petli tworzy n potokow nienazwanych
            perror("pipe");
            return 1;
        }

        pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        } else if (pid == 0) { // Proces potomny
            close(pipes_fd[i][0]); // Zamykamy czytanie z potoku

            double part_integral = calculate_integral(start + i * (end - start) / num_processes,
                                                       start + (i + 1) * (end - start) / num_processes,
                                                       width);
                                                       
            write(pipes_fd[i][1], &part_integral, sizeof(double)); // zapis do potoku
            close(pipes_fd[i][1]);
            exit(0);
        }
    }

    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    for (int i = 0; i < num_processes; i++) {
        close(pipes_fd[i][1]); // Zamykamy pisanie do potoku
        double part_integral;
        read(pipes_fd[i][0], &part_integral, sizeof(double)); // odczytuje dane z kolenjnych potoków
        close(pipes_fd[i][0]);
        total_integral += part_integral;
    }

    printf(" ================================================================ \n");
    printf("Dokładność: %e \nLiczba procesow: %ld \nCałka oznaczona: %.4lf \n \n", width, num_processes, total_integral);

    return 0;
}