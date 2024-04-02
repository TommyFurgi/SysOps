CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: sender catcher

catcher:
	$(CC) $(CFLAGS) -o catcher catcher.c

sender:
	$(CC) $(CFLAGS) -o sender sender.c	

clean:
	rm -f catcher sender

