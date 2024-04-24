#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define IPC_KEY 0x12345678
#define MAX_CLIENTS 10
#define MAX_MESSAGE_SIZE 256

struct message {
    long type;
    int client_id;
    char text[MAX_MESSAGE_SIZE];
};

int main() {
    int server_queue_id;

    int client_ids[MAX_CLIENTS];
    for(int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = -1;
    }

    int client_count = 0; 
    struct message msg;

    // Tworzenie kolejki serwera
    server_queue_id = msgget(IPC_KEY, IPC_CREAT | 0666);
    if (server_queue_id == -1) {
        perror("Błąd podczas tworzenia kolejki serwera");
        exit(EXIT_FAILURE);
    }

    printf("Serwer jest gotowy do działania.\n");

    while (1) {

        if (msgrcv(server_queue_id, &msg, sizeof(struct message) - (sizeof(long)), 0, 0) == -1) { // odbieranie komunikatów przez serwer
            perror("Błąd podczas odbierania komunikatu");
            exit(EXIT_FAILURE);
        }


        if (msg.type == 1) {
            int empty_index = -1; 

            // Szukanie pierwszego pustego miejsca w tablicy identyfikatorów klientów
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_ids[i] == -1) {
                    empty_index = i;
                    break;
                }
            }

            // Sprawdzenie czy znaleziono puste miejsce
            if (empty_index != -1) {

                // Zapisywanie identyfikatora klienta w tablicy
                client_ids[empty_index] = msg.client_id;
                printf("Klient %d dołączył.\n", msg.client_id);
                client_count++;

                msg.client_id = empty_index;

                if (msgsnd(client_ids[empty_index], &msg, sizeof(struct message) - (sizeof(long)), 0) == -1) {
                    perror("Błąd podczas wysyłania komunikatu do klienta");
                }
            } else {
                printf("Osiągnięto limit klientów. Nie można dodać więcej klientów.\n");
            }


        } else if (msg.type == 2) {
            printf("Serwer otrzymał wiadomość od klineta %d \n", msg.client_id);

            for (int i = 0; i < MAX_CLIENTS; i++) {

                if (client_ids[i] != -1 && i != msg.client_id){
                    if (msgsnd(client_ids[i], &msg, sizeof(struct message) - (sizeof(long)), 0) == -1) {
                        perror("Błąd podczas wysyłania komunikatu do klienta");
                    }
                }
                
            }
        
        } else if (msg.type == 3) {
            client_ids[msg.client_id] = -1;
            client_count -= 1;
            printf("Koniec komunikacji z klientem %d \n", msg.client_id);

        } else {
            perror("Błąd, nieznany typ komunikatu");
        }

    }

    if (msgctl(server_queue_id, IPC_RMID, NULL) == -1) {
        perror("Błąd podczas zamykania kolejki serwera");
        exit(EXIT_FAILURE);
    }

    return 0;
}
