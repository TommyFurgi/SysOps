CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

test_server: server
	./server 8080

test_client1: client
	./client client1 127.0.0.1 8080

test_client2: client
	./client client2 127.0.0.1 8080

clean:
	rm -f server client