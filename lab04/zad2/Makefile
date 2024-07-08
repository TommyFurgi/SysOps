CC = gcc
CFLAGS = -Wall -Wextra 

.PHONY: all clean

all: main

main:
	$(CC) $(CFLAGS) main.c -o main

test_main: main
	./main ../zad1

clean:
	rm -f main