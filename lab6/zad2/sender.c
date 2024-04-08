#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

    if (mkfifo("potok1", 0666) == -1 || mkfifo("potok2", 0666) == -1) { // tworzy dwa potoki nazwamane
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    double start, end;

    printf("Podaj przedział całkowania (początek koniec): ");
    scanf("%lf %lf", &start, &end);

    
    int fd_output = open("potok1", O_WRONLY); // otwiera potok nazwany do zapisu
    if (fd_output == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fd_output, &start, sizeof(double)) == -1 || write(fd_output, &end, sizeof(double)) == -1) { // wysyła przedział przez potok
        perror("write"); 
        close(fd_output);
        exit(EXIT_FAILURE);
    }   
    close(fd_output);
    double integral;

    // potok nazwany do odczytu
    int fd_input = open("potok2", O_RDONLY);
    if (fd_input == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // wartość całki z potoku
    if (read(fd_input, &integral, sizeof(double)) == -1) {
        perror("read");
        close(fd_input);
        exit(EXIT_FAILURE);
    }

    close(fd_input);
    printf("Odebrana wartość całki: %lf\n", integral);

    if (unlink("potok1") == -1 || unlink("potok2") == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
