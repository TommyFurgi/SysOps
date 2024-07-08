CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: signals

signals: main.c
	$(CC) $(CFLAGS) -o signals main.c

test_signals: signals
	@echo "========= ignore: ==========:"
	./signals ignore
	@echo "========= handler: =========:"
	./signals handler
	@echo "=========== mask: ==========:"
	./signals mask
	@echo "=========== none: ==========:"
	./signals none

clean:
	rm -f signals