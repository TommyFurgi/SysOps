CC = gcc
CFLAGS = -Wall -Wextra 

.PHONY: all clean

all: create_processes

create_processes:
	 $(CC) $(CFLAGS) main.c -o create_processes

test_creating_processes: create_processes
	./create_processes 5

clean:
	rm -f create_processes