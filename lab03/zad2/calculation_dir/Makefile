CC = gcc
CFLAGS = -Wall -Wextra 
INPUT = input.txt
OUTPUT1 = output1.txt
OUTPUT2 = output2.txt
TIME_RESULT = time_result.txt

.PHONY: all clean

all: reverse_file_by_buffer reverse_file_by_byte 

reverse_file_by_buffer: 
	$(CC) $(CFLAGS) main.c -o reverse_file_by_buffer -D BUFFER_REVERSE

reverse_file_by_byte:
	$(CC) $(CFLAGS) main.c -o reverse_file_by_byte -D BYTE_REVERSE


 
run_buffer: reverse_file_by_buffer
	./reverse_file_by_buffer $(INPUT) $(OUTPUT1) 

run_byte: reverse_file_by_byte
	./reverse_file_by_byte $(INPUT) $(OUTPUT1) 


test_byte: reverse_file_by_byte
	./reverse_file_by_byte $(INPUT) $(OUTPUT1) 
	./reverse_file_by_byte $(OUTPUT1) $(OUTPUT2)
	diff -s $(INPUT) $(OUTPUT2)

test_buffer: reverse_file_by_buffer
	./reverse_file_by_buffer $(INPUT) $(OUTPUT1) 
	./reverse_file_by_buffer $(OUTPUT1) $(OUTPUT2)
	diff -s $(INPUT) $(OUTPUT2)

test_time_byte: reverse_file_by_byte
	/usr/bin/time -v ./reverse_file_by_byte $(INPUT) $(OUTPUT1) > $(TIME_RESULT) 2>&1


test_time_buffer: reverse_file_by_buffer
	/usr/bin/time -v ./reverse_file_by_buffer $(INPUT) $(OUTPUT1) > $(TIME_RESULT) 2>&1




clean:
	rm -f reverse_file_by_buffer reverse_file_by_byte
	rm -f output1.txt output2.txt time_result.txt
