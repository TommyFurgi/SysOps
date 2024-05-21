#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define SHARED_KEY 0x12345678
#define USERS_NUMBER 5 // liczba użytkowników
#define QUEUE_SIZE 10 // rozmiar kolejki zleceń
#define TEXT_SIZE 10 // rozmiar tekstu do druku


// Struktura opisująca zadanie wydruku
struct PrintJob {
    char text[TEXT_SIZE];
    int id;
};

// Struktura opisująca pamięć wspólną
struct SharedMemory {
    struct PrintJob queue[QUEUE_SIZE];
    int queueFront;
    int queueRear;
};

// Definicje semaforów
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

int semid; // ID zbioru semaforów
int shmid; // ID pamięci wspólnej
struct SharedMemory *sharedMem; // Wskaźnik do pamięci wspólnej

// Funkcja inicjalizująca semafory
void initSemaphores() {
    semid = semget(SHARED_KEY, 0, 0666);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, SEM_MUTEX, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, SEM_EMPTY, SETVAL, QUEUE_SIZE) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, SEM_FULL, SETVAL, 0) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

// Funkcja zamykająca semafor
void semWait(int semnum) {
    struct sembuf semops;
    semops.sem_num = semnum;
    semops.sem_op = -1;
    semops.sem_flg = 0;

    if (semop(semid, &semops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

// Funkcja otwierająca semafor
void semSignal(int semnum) {
    struct sembuf semops;
    semops.sem_num = semnum;
    semops.sem_op = 1;
    semops.sem_flg = 0;

    if (semop(semid, &semops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

// Funkcja użytkownika
void user(int id) {
    srand((unsigned int)getpid());

    while (1) {
        // Generowanie zadania wydruku
        struct PrintJob job;
        for (int i = 0; i < TEXT_SIZE; i++) {
            job.text[i] = 'a' + rand() % 26;
        }
        job.id = id;

        // Dodanie zadania do kolejki
        semWait(SEM_EMPTY);
        semWait(SEM_MUTEX);

            sharedMem->queue[sharedMem->queueRear] = job;
            sharedMem->queueRear = (sharedMem->queueRear + 1) % QUEUE_SIZE;
            printf("User %d wrote in queue \n", id);
        

        semSignal(SEM_MUTEX);
        semSignal(SEM_FULL);

        // Czekanie losową liczbę sekund
        sleep(rand() % 5 + 1);
    }
}

int main() {

    // Dostęp do wspólnej pamięci
    shmid = shmget(SHARED_KEY, sizeof(struct SharedMemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    sharedMem = (struct SharedMemory *)shmat(shmid, NULL, 0);
    if (sharedMem == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    // Inicjalizacja semaforów
    initSemaphores();

    // Tworzenie procesów użytkowników
    for (int i = 0; i < USERS_NUMBER; i++) {
        if (fork() == 0) {
            user(i);
        }
    }

    // Oczekiwanie na zakończenie wszystkich procesów potomnych
    for (int i = 0; i < USERS_NUMBER; i++) {
        wait(NULL);
    }

    // Usuwanie pamięci wspólnej
    if (shmdt(sharedMem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // usnunięcie zioru semafor
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
