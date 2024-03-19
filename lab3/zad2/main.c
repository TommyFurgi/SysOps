#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1024
char path_buffer[BUFFER_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s <katalog>\n", argv[0]);
        return -1;
    }

    // otwiera katalog
    DIR *dir_handler = opendir(argv[1]);
    if (dir_handler == NULL) {
        perror("Błąd przy otwieraniu katalogu");
        return -1;
    }


    size_t dir_path_length = strlen(argv[1]);

    if(dir_path_length + 1 >= BUFFER_SIZE - 1){
        printf("Podana ściezka jest za długa \n");
        return -1;
    }
    memcpy(path_buffer, argv[1], dir_path_length);

    // scieżka musi kończyć się na /
    if(path_buffer[dir_path_length - 1] != '/'){
        path_buffer[dir_path_length] = '/';
        dir_path_length += 1;
    }

    struct stat file_stat;
    long long total_size = 0;

    // bierze pierwszy plik
    struct dirent* entry_file = readdir(dir_handler);


    while (entry_file != NULL) {
        snprintf(path_buffer, BUFFER_SIZE, "%s/%s", argv[1], entry_file->d_name);

        // stat() - służy do pobierania informacji o pliku
        if (stat(path_buffer, &file_stat) == -1) {
            error("Błąd przy pobieraniu informacji o pliku");
            continue;
        }

        if(!S_ISDIR(file_stat.st_mode)){
            printf("%8ld %s\n",file_stat.st_size, path_buffer);
            total_size += file_stat.st_size;
        }

        entry_file = readdir(dir_handler);
    }

    closedir(dir_handler);

    printf("Sumaryczny rozmiar wszystkich plików: %lld\n", total_size);

    return 0;
}
