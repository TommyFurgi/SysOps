#include <stdio.h>
#include <stdio.h>

#ifdef BYTE_REVERSE
    void reverse_by_byte(FILE *input, FILE *output){
        fseek(input, -1, SEEK_END); // ustawienie wskaźnika w pliku
        long bytes = ftell(input) + 1; // pobiera ile bajtów ma plik

        char c;
        while (bytes > 0) {
            c = fgetc(input);
            fwrite(&c, sizeof(char), 1, output);
            bytes -= 1;
            fseek(input, -2, SEEK_CUR);
        }
    }
#endif

#ifdef BUFFER_REVERSE
    #define BUFFER_SIZE 5
    void reverse_by_buffer(FILE *input, FILE *output){

        fseek(input, 0, SEEK_END);
        long bytes = ftell(input);

        long bytes_to_take;
        char buffer[BUFFER_SIZE];
        char c;

        while (bytes > 0){
            if (bytes < BUFFER_SIZE)
                bytes_to_take = bytes;
            else 
                bytes_to_take = BUFFER_SIZE;

            fseek(input, -bytes_to_take, SEEK_CUR);

            size_t bytes_read = fread(buffer, sizeof(char), bytes_to_take, input);
            for (int i=0; i<bytes_to_take/2; i++){
                c = buffer[i];
                buffer[i] = buffer[bytes_to_take - i -1];
                buffer[bytes_to_take - i -1] = c;
            }

            fwrite(buffer, sizeof(char), bytes_read, output);

            fseek(input, -BUFFER_SIZE, SEEK_CUR);
            bytes -= bytes_to_take;
        }
    }
#endif

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Sposób użycia: %s <plik_wejściowy> <plik_wyjściowy>\n", argv[0]);
        return -1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Błąd przy otwieraniu pliku wejściowego");
        return -1;
    }

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        perror("Błąd przy otwieraniu pliku wyjściowego");
        fclose(input_file);
        return -1;
    }

    #ifdef BYTE_REVERSE
        reverse_by_byte(input_file, output_file);
    #endif

    #ifdef BUFFER_REVERSE
        reverse_by_buffer(input_file, output_file);
    #endif

    fclose(input_file);
    fclose(output_file);

    return 0;   
}