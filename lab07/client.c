#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define IPC_KEY 0x12345678
#define MAX_MESSAGE_SIZE 256

struct message {
    long type;
    int client_id;
    char text[MAX_MESSAGE_SIZE];
};

int main() {
    int server_queue_id;
    int client_queue_id;
    struct message msg;
    char input[MAX_MESSAGE_SIZE];

    pid_t child = fork();
    if (child < 0) {
        perror("Error creating child process");
    }

    // // Tworzenie unikalnego klucza dla kolejki klienta
    key_t client_key = ftok(".", getpid());

    // Tworzenie kolejki klienta
    client_queue_id = msgget(client_key, IPC_CREAT | 0666);
    if (client_queue_id == -1) {
        perror("Błąd podczas tworzenia kolejki klienta");
        exit(EXIT_FAILURE);
    }

    // Otwieranie kolejki serwera
    server_queue_id = msgget(IPC_KEY, 0666);
    if (server_queue_id == -1) {
        perror("Błąd podczas otwierania kolejki serwera");
        exit(EXIT_FAILURE);
    }

    // Wysyłanie INIT do serwera wraz z kluczem kolejki klienta
    msg.type = 1;
    msg.client_id = client_queue_id;
    if (msgsnd(server_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0) == -1) {
        perror("Błąd podczas wysyłania komunikatu INIT do serwera");
        exit(EXIT_FAILURE);
    }

    // Odbieranie identyfikatora klienta od serwera
    if (msgrcv(client_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0, 0) == -1) {
        perror("Błąd podczas odbierania identyfikatora klienta od serwera");
        exit(EXIT_FAILURE);
    }

    msg.type = 2;
    printf("Klinet otrzymal identyfikator: %d\n", msg.client_id);

    if (child == 0){
        

        while(1) {
            if (msgrcv(client_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0, 0) == -1) {
                perror("Błąd podczas odbierania identyfikatora klienta od serwera");
                exit(EXIT_FAILURE);
            }
            printf("Proces potomny otrzymał wiadomosc: %s \n", msg.text);

        }


    } else {
        sleep(1);

        while (1) {
            printf("Wpisz wiadomosc: ");
            scanf(" %[^\n]%*c", input);
            strcpy(msg.text, input);

            if (!strcmp(input, "exit"))
                break;

            if (msgsnd(server_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0) == -1) {
                perror("Błąd podczas wysyłania komunikatu do serwera");
                exit(EXIT_FAILURE);
            }

            sleep(2);
        }
    
        msg.type = 3; // zakoncz komunikacje
        if (msgsnd(server_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0) == -1) {
            perror("Błąd podczas wysyłania komunikatu do serwera");
            exit(EXIT_FAILURE);
        }

        if (msgctl(client_queue_id, IPC_RMID, NULL) == -1) {
            perror("Błąd podczas zamykania kolejki serwera");
            exit(EXIT_FAILURE);
        }
        

        

        wait(NULL);
    }

   

    return 0;
}
