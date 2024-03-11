CXX = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

.PHONY: all countdown clean

all: countdown test

test: test.o
	$(CXX) $(CFLAGS) -o test test.o

test.o: test.c
	$(CXX) $(CFLAGS) -c test.c

countdown: countdown.o function.o
	$(CXX) $(CFLAGS) -o countdown countdown.o function.o

function.o: function.c
	$(CXX) $(CFLAGS) -c function.c

countdown.o: countdown.c
	$(CXX) $(CFLAGS) -c countdown.c


clean:
	rm -f countdown countdown.o function.o
	rm -f test test.o


