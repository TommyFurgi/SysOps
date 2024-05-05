#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define SHARED_KEY 0x12345678
#define PRINTERS 2 // liczba drukarek
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
    semid = semget(SHARED_KEY, 3, IPC_CREAT | 0666);
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

// Funkcja drukarki
void printer(int id) {
    char filename[20];
    sprintf(filename, "printer_%d.txt", id);

    FILE* fd = fopen(filename, "w");
    if (fd == NULL) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    struct PrintJob job;
    
    while (1) {
        // Pobranie zadania z kolejki
        semWait(SEM_FULL);
        semWait(SEM_MUTEX);
        
            job = sharedMem->queue[sharedMem->queueFront];
            sharedMem->queueFront = (sharedMem->queueFront + 1) % QUEUE_SIZE;
            printf("Printer %d read from queue \n", id);

        semSignal(SEM_MUTEX);
        semSignal(SEM_EMPTY);
        

        fprintf(fd, "Printer %d printing text from user %d: ", id, job.id);
        
        for (int i = 0; i < 10; i++) {
            fprintf(fd, "%c", job.text[i]);
            fflush(fd);
            sleep(1);
        }
        fprintf(fd, "\n");
    }
}

int main() {
    srand(time(NULL));

    // Inicjalizacja pamięci wspólnej
    int shmid = shmget(SHARED_KEY, sizeof(struct SharedMemory), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    sharedMem = (struct SharedMemory *)shmat(shmid, NULL, 0);
    if (sharedMem == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    sharedMem->queueFront = 0;
    sharedMem->queueRear = 0;

    // Inicjalizacja semaforów
    initSemaphores();

    // Tworzenie procesów drukarek
    for (int i = 0; i < PRINTERS; i++) {
        if (fork() == 0) {
            printer(i);
        }
    }

    // Oczekiwanie na zakończenie wszystkich procesów potomnych
    for (int i = 0; i < PRINTERS; i++) {
        wait(NULL);
    }

    // Usuwanie pamięci wspólnej
    if (shmdt(sharedMem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    
    // Usuwanie semaforów
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 1, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 2, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    

    return 0;
}
