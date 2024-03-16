CC = gcc
CFLAGS = -Wall -Wextra 

.PHONY: all clean

all: calculate_files_size

calculate_files_size: 
	$(CC) $(CFLAGS) main.c -o main

test_claculation: calculate_files_size
	@echo "============================== main.c output: ==============================:"
	./main ../test_directory/
	@echo "============================== wc --bytes output: ==============================:"
	wc --bytes ../test_directory/*


clean:
	rm -f main
