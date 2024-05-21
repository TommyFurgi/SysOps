#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;

// inicjalizacja
pthread_mutex_t reindeers_mutexes[9] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

// inicjalizacja mutexa odpowiedzialnego za powrot
int reindeers_back_count = 0;
pthread_mutex_t reindeers_back_mutex = PTHREAD_MUTEX_INITIALIZER;

// deklaracja wztków 
pthread_t santa_thread;
pthread_t reindeers_threads[9];

void* reindeer_thread_handler(void* arg) {
    int id = *(int*)arg;
    // ustawienie na typ cancel, bu moc w kazdym momentcie przerwac watek
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // wyslanie renifera na wakacje
    pthread_mutex_lock(&reindeers_mutexes[id]);

    while(1) {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&reindeers_back_mutex);
        printf("Renifer %d: czeka %d reniferów \n", id, reindeers_back_count);

        // zwiększ liczbe oczeujących reniferów 
        reindeers_back_count++;
        if (reindeers_back_count == 9) {
            printf("Renifer %d: budzę Mikołaja \n", id);
            // obudz watek mikolaja
            pthread_cond_signal(&santa_cond);
            reindeers_back_count = 0;
        }

        pthread_mutex_unlock(&reindeers_back_mutex);

        // zablokuj ponownie reindeers_mutexes[id] by wstrzymać wątek
        // oczekuje na pozostale renifery i powrot
        pthread_mutex_lock(&reindeers_mutexes[id]);

        printf("Renifer %d: lecę na wakacje \n", id);
    }  

    return NULL;
}

void* santa_thread_handler(void* arg) {

    for (int i = 0; i < 4; i++) {
        // watek oczekuje na wybudzenie
        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Mikołaj: budzę się\n");

        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2);

        // wybudza wszystkie watki reniferow
        for (int j = 0; j < 9; j++) {
            pthread_mutex_unlock(&reindeers_mutexes[j]);
        }

        printf("Mikołaj: zasypiam\n");
    }

    // usuwa watki reniferow
    for (int j = 0; j < 9; j++) {
        pthread_cancel(reindeers_threads[j]);
    }

    return NULL;
}

int main() {
    int ids[9];
    
    // tworzenie watkow
    pthread_create(&santa_thread, NULL, santa_thread_handler, NULL);
    for (int i = 0; i < 9; i++) {
        ids[i] = i;
        pthread_create(&reindeers_threads[i], NULL, reindeer_thread_handler, &ids[i]);
    }

    // czekanie na zakonczenie watkow
    pthread_join(santa_thread, NULL);
    for (int i = 0; i < 9; i++) {
        pthread_join(reindeers_threads[i], NULL);
    }

    printf("Koniec\n");
}