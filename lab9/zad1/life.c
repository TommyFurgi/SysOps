#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "grid.h"
#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define GRID_SIZE_X 20
#define GRID_SIZE_Y 20

char *foreground;
char *background;

typedef struct {
    int start_index;
    int end_index;

} ThreadData;

void handler(int sig)
{
    //do nothing
}

void* thread_compute(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int start = data->start_index;
    int end = data->end_index;

    signal(SIGUSR1, handler);

    while (true) {
        pause();

        for (int i = start; i < end; i++) {
			int row = i / GRID_SIZE_X; 
			int col = i % GRID_SIZE_X; 

			/* update cell in background array */
			background[i] = is_alive(row, col, foreground);
		}
    }

    pthread_exit(NULL);
}


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int NUM_THREADS = strtol(argv[1], NULL, 10);

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); 

    foreground = create_grid();
    background = create_grid();
    init_grid(foreground);

    // Inicjalizacja danych wątków obliczeniowych
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int cells_per_thread = (int)ceil(GRID_SIZE_X * GRID_SIZE_Y / NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; i++) { // przydzielenie każdemu wątkowi komórek 
		thread_data[i].start_index = i * cells_per_thread;
		thread_data[i].end_index = MIN((i + 1) * cells_per_thread, GRID_SIZE_X * GRID_SIZE_Y); 

        // uruchomienie wątku
		pthread_create(&threads[i], NULL, thread_compute, &thread_data[i]);
	}

	init_grid(foreground);

    while (true) {
        draw_grid(foreground);

        // odblokowanie wątków
        for(int i = 0; i < NUM_THREADS; i++) {
			pthread_kill(threads[i], SIGUSR1);
		}
		usleep(500 * 1000);


        // update_grid(foreground, background);

        char *tmp = foreground;
        foreground = background;
        background = tmp;
    }

    // Usunięcie zasobów i zakończenie programu
    endwin(); 
    destroy_grid(foreground);
    destroy_grid(background);

    return 0;
}
