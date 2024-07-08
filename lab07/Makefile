CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: server client

serwer:
	$(CC) $(CFLAGS) server.c -o server

client:
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f client server