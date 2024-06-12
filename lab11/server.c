#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 1024
#define ALIVE_INTERVAL 20

// struktura do przechowywania klienta
typedef struct {
    int sockfd;
    char id[20];
    int active;
} Client;

// lista z klientami i mutex odpowiedzialny za kontrole dostępu do tablicy
Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// funkcja do pobrania obecnego czasu
void get_current_time(char *buffer, int buffer_size) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void *handle_client(void *arg) {
    int sockfd = *((int *)arg);  // pobiera deskrytptor
    char buffer[BUFFER_SIZE];
    char id[20];
    int index = -1;

    recv(sockfd, id, sizeof(id), 0); // odczytuje identyfikator klienta

    pthread_mutex_lock(&clients_mutex);
    // Sprawdź, czy klient o podanym id już istnieje
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, id) == 0) {
            printf("Client with id %s already exists\n", id);
            send(sockfd, "ERROR: ID already in use\n", 25, 0);
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            pthread_mutex_unlock(&clients_mutex);
            return NULL;
        }
    }
    // Dodanie klienta do tablicy
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].sockfd = sockfd;
            strcpy(clients[i].id, id);
            clients[i].active = 1;
            index = i;
            break;
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);

    if (index == -1) {
        printf("Max clients limit reached\n");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        return NULL;
    }

    printf("Client %s connected\n", id);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0); // oczytanie wiadomości 
        if (bytes_received <= 0) {
            break;
        }

        if (strncmp(buffer, "LIST", 4) == 0) {
            list_clients(sockfd);
        } else if (strncmp(buffer, "2ALL ", 5) == 0) {
            char message[BUFFER_SIZE + 100];
            char timestamp[100];
            get_current_time(timestamp, sizeof(timestamp));
            sprintf(message, "[%s] %s: %s", timestamp, id, buffer + 5);
            send_to_all(message, id);
        } else if (strncmp(buffer, "2ONE ", 5) == 0) {
            char *recipient_id = strtok(buffer + 5, " ");
            char *message = strtok(NULL, "");
            if (recipient_id && message) {
                char full_message[BUFFER_SIZE + 100];
                char timestamp[100];
                get_current_time(timestamp, sizeof(timestamp));
                sprintf(full_message, "[%s] %s: %s", timestamp, id, message);
                send_to_one(full_message, recipient_id);
            }
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            break;
        }
    }

    printf("Client %s disconnected\n", id);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd); // zamknięcie deskryptora
    pthread_mutex_lock(&clients_mutex);
    clients[index].active = 0;
    pthread_mutex_unlock(&clients_mutex);

    return NULL;
}

// funkcja do wysyłania wiadomości do wszystkich klientów
void send_to_all(char *message, char *sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, sender) != 0) {
            send(clients[i].sockfd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// funkcja do wysyłania wiadomości do jednego klienta
void send_to_one(char *message, char *recipient_id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, recipient_id) == 0) {
            send(clients[i].sockfd, message, strlen(message), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// wysyła do jednego klienta listę obcnie podłączonych klientów(przechodzi po tablicy i wysyła po kolei)
void list_clients(int sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            send(sockfd, clients[i].id, strlen(clients[i].id), 0);
            send(sockfd, "\n", 1, 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// funkcja uruchamiana w osobnym wątku do pingowania klientów
void *alive_checker(void *arg) {
    while (1) {
        sleep(ALIVE_INTERVAL);
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                // Przekazuje wiadomość "ALIVE" do wszystkich aktywnych klientów
                if (send(clients[i].sockfd, "ALIVE", 5, 0) <= 0) {
                    // Jeśli wysłanie wiadomości zwróciło błąd, klient jest uznawany za nieaktywnego
                    clients[i].active = 0;
                    close(clients[i].sockfd);
                    printf("Client %s disconnected due to no response\n", clients[i].id);
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    
    return NULL;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_sockfd, new_sockfd, port;
    struct sockaddr_in server_addr, client_addr; // struktury przechowujące adresy serwera i klienta
    socklen_t client_addr_len = sizeof(client_addr);

    port = atoi(argv[1]);

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0); // tworzy gniazdo TCP/IP
    if (server_sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // przypisuje adres do gniazda serwera
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    // ustawia gniazdo w tryb nasłuchiwania
    if (listen(server_sockfd, 5) < 0) {
        perror("Listen failed");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);

    pthread_t alive_thread;
    pthread_create(&alive_thread, NULL, alive_checker, NULL);

    while (1) {
        // akceptuje nowe połączenie
        new_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_sockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // dla każdego nowego połączenia tworzy nowy wątek
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void *)&new_sockfd);
    }

    shutdown(server_sockfd, SHUT_RDWR);
    close(server_sockfd);
    return 0;
}