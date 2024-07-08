CXX = gcc

cflags.debug := -g -O0
cflags.release := -O2

CFLAGS = -Wall -Wextra

.PHONY: all static_library shared_library client_static client_shared client_dynamic clean

all: client_static client_shared client_dynamic 

# Creating static library with .a extension
static_library:
	$(CXX) -c collatz.c 
	ar rcs libcollatz.a -o collatz.o

# Creating dynamic library with .so extension
shared_library:
	$(CXX) -fPIC -c collatz.c
	$(CXX) -shared collatz.o -o libcollatz.so

client_static: static_library
	$(CXX) $(CFLAGS) client.c -L. -lcollatz -o client_static

client_shared: shared_library
	$(CXX) $(CFLAGS) client.c -L. -lcollatz -Wl,-rpath=. -o client_shared

client_dynamic: shared_library
	$(CXX) $(CFLAGS) client.c -o client_dynamic -ldl -D DYNAMIC_CLIENT


test_static: client_static
	./client_static
test_shared: client_shared
	./client_shared
test_dynamic: client_dynamic
	./client_dynamic

objdump_static: client_static
	objdump -d client_static > objdump_static.txt

objdump_shared: client_shared
	objdump -d client_shared > objdump_shared.txt

objdump_dynamic: client_dynamic
	objdump -d client_dynamic > objdump_dynamic.txt

clean:
	rm -f *.a *.so *.o 
	rm -f client_static client_shared client_dynamic
	rm -f objdump_static.txt objdump_shared.txt objdump_dynamic.txt
