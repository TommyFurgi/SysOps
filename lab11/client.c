#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;

void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0); // obiera wiadomość
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

// handler na sygał CTRL + C
void sigint_handler(int sig) {
    char buffer[] = "STOP";
    send(sockfd, buffer, strlen(buffer), 0);
    close(sockfd);
    exit(0);
}

int main(int argc, char** argv) {
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ID> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *id = argv[1];
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);


    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // tworzy gniazdo TCP/IP
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) { // konwertuje adres IP z formatu tekstowego do binarnego
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // nawiązuje połączenie z serwerem używając gniazda klienta (sockfd) i adresu serwera (server_addr)
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { 
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // wysyła identyfikator do serwera
    send(sockfd, id, strlen(id), 0);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL); // tworzy wątek do odbierania wiadomości

    signal(SIGINT, sigint_handler);

    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin); // czyta ze standwadowego wejścia
        buffer[strcspn(buffer, "\n")] = 0;
        if (strncmp(buffer, "STOP", 4) == 0) {
            send(sockfd, buffer, strlen(buffer), 0); 
            break;
        }
        send(sockfd, buffer, strlen(buffer), 0); // wysyła wiadomość
    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd); // zamyka gniazdo
    return 0;
}

