#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 1024
#define ALIVE_INTERVAL 20

typedef struct {
    struct sockaddr_in addr;
    char id[20];
    int active;
} Client;

Client clients[MAX_CLIENTS];

void get_current_time(char *buffer, int buffer_size) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void *alive_checker(void *arg) {
    int sockfd = *((int *)arg);
    while (1) {
        sleep(ALIVE_INTERVAL);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (sendto(sockfd, "ALIVE", 5, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr)) < 0) {
                    clients[i].active = 0;
                    printf("Client %s disconnected due to no response\n", clients[i].id);
                }
            }
        }
    }
    return NULL;
}

bool add_client(Client *clients, int max_clients, struct sockaddr_in client_addr, char *id) {
    for (int i = 0; i < max_clients; i++) {
        if (!clients[i].active) {
            clients[i].addr = client_addr;
            strcpy(clients[i].id, id);
            clients[i].active = true;
            printf("New client with id: %s connected \n", id);
            return true;
        }
    }
    printf("Too much clients \n");
    return false; // Nie udało się dodać klienta, lista pełna
}

void remove_client(Client *clients, int max_clients, char *id) {
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].active && strcmp(clients[i].id, id) == 0) {
            clients[i].active = false;
            printf("Client with id: %s disconected \n", id);
            return;
        }
    }
}

int compare_sockaddr_in(struct sockaddr_in *a, struct sockaddr_in *b) {
    return (a->sin_family == b->sin_family &&
            a->sin_addr.s_addr == b->sin_addr.s_addr &&
            a->sin_port == b->sin_port);
}

char* get_id(struct sockaddr_in cliaddr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (compare_sockaddr_in(&clients[i].addr, &cliaddr)) {
            return clients[i].id;
        }
    }
    return NULL;
}

void handle_client(server_sockfd) {
    int sockfd = server_sockfd;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    while (1) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);


        if (strncmp(buffer, "LIST", 4) == 0) {
            list_clients(sockfd, &cliaddr);
        } else if (strncmp(buffer, "2ALL ", 5) == 0) {
            char *id = get_id(cliaddr);
            char message[BUFFER_SIZE + 100];
            char timestamp[100];
            get_current_time(timestamp, sizeof(timestamp));
            sprintf(message, "[%s] %s: %s", timestamp, id, buffer + 5);
            send_to_all(message, id, sockfd);
        } else if (strncmp(buffer, "2ONE ", 5) == 0) {
            char *id = get_id(cliaddr);
            char *recipient_id = strtok(buffer + 5, " ");
            char *message = strtok(NULL, "");
            if (recipient_id && message) {
                char full_message[BUFFER_SIZE + 100];
                char timestamp[100];
                get_current_time(timestamp, sizeof(timestamp));
                sprintf(full_message, "[%s] %s: %s", timestamp, id, message);
                send_to_one(full_message, recipient_id, sockfd);
            }
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            char *id = get_id(cliaddr);
            remove_client(clients, MAX_CLIENTS, id);
        } else if (strncmp(buffer, "NEW ", 4) == 0) {
            char new_id[20];
            sscanf(buffer + 4, "%s", new_id);
            add_client(clients, MAX_CLIENTS, cliaddr, new_id);
        }
    }

    return NULL;
}


void send_to_all(char *message, char *sender, int sockfd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, sender) != 0) {
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
}

void send_to_one(char *message, char *recipient_id, int sockfd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, recipient_id) == 0) {
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            break;
        }
    }
}

void list_clients(int sockfd, struct sockaddr_in *client_addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            sendto(sockfd, clients[i].id, strlen(clients[i].id), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
            sendto(sockfd, "", 1, 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int port = atoi(argv[1]);

    // tworzy gniazdo UDP
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // definicja adressu taka sama jak TCP
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // przypisuje adres do gniazda serwera
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);

    pthread_t alive_thread; // wątek do powiadamiania
    pthread_create(&alive_thread, NULL, alive_checker, (void *)&server_sockfd);

    handle_client(server_sockfd); // funkcja serwera

    shutdown(server_sockfd, SHUT_RDWR);
    close(server_sockfd);
    return 0;
}
