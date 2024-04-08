#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



double function(double x) {
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


int main() {
    
    int fd_input = open("potok1", O_RDONLY);
    if (fd_input == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    double start, end;
    if (read(fd_input, &start, sizeof(double)) == -1 || read(fd_input, &end, sizeof(double)) == -1) { // odbiera przedział z potoku
        perror("read");
        close(fd_input);
        exit(EXIT_FAILURE);
    }

    close(fd_input);
    printf("Przedział od %lf do %lf \n", start, end);


    double width = 0.0001;
    double integral = calculate_integral(start, end, width);
    printf("Obliczona wartość całki: %lf\n", integral);

    int fd_output = open("potok2", O_WRONLY); // otwiera potok
    if (fd_output == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fd_output, &integral, sizeof(double)) == -1) { // zapisuje wartość do potoku
        perror("write");
        close(fd_output);
        exit(EXIT_FAILURE);
    }

    close(fd_output);
    return 0;
}
