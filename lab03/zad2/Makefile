CC = gcc
CFLAGS = -Wall -Wextra 

.PHONY: all clean

all: calculate_files_size

calculate_files_size: 
	$(CC) $(CFLAGS) main.c -o main

test_claculation: calculate_files_size
	@echo "============================== main.c output: ==============================:"
	./main ./calculation_dir/
	@echo "============================== wc --bytes output: ==============================:"
	wc --bytes ./calculation_dir/*


clean:
	rm -f main
