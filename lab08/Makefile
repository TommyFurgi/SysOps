CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: user printer

user: user.c
	$(CC) $(CFLAGS) -o user user.c

printer: printer.c
	$(CC) $(CFLAGS) -o printer printer.c


clean:
	rm -f user printer printer_0.txt printer_1.txt