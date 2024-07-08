CC = gcc
CFLAGS = -Wall

all: life 
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -lncursesw
	
test: life
	./life 3

clean:
	rm -f life 
