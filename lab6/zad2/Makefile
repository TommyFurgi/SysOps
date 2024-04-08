CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: integral sender

integral: integral.c
	$(CC) $(CFLAGS) -o integral integral.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f integral sender potok1 potok2