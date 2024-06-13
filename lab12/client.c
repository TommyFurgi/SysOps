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
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

void sigint_handler(int sig) {
    char buffer[] = "STOP";
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
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

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Sending ID to the server
    char new_message[BUFFER_SIZE];
    sprintf(new_message, "NEW %s", id);
    sendto(sockfd, new_message, strlen(new_message), 0, (struct sockaddr *)&server_addr, addr_len);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    signal(SIGINT, sigint_handler);

    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Removing the newline character
        if (strncmp(buffer, "STOP", 4) == 0) {
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
            break;
        }
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
    }

    close(sockfd);
    return 0;
}
