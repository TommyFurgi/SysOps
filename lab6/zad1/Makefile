CC = gcc
CFLAGS = -Wall -Wextra
TIME_FILE = time.txt

.PHONY: all clean

all: integral

integral: integral.c
	$(CC) $(CFLAGS) -o integral integral.c


test: integral
	/usr/bin/time -v ./integral 0.00000001 5000 > $(TIME_FILE) 2>&1
	/usr/bin/time -v ./integral 0.000000001 30 >> $(TIME_FILE) 2>&1
	/usr/bin/time -v ./integral 0.000000001 100 >> $(TIME_FILE) 2>&1


clean:
	rm -f integral time.txt