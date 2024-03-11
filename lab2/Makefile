CXX = gcc

# disable all optimizations and add debug information in debug build
cflags.debug := -g -O0
# enable O2 optimization in release mode
cflags.release := -O2

CFLAGS = -Wall -Wextra -g

.PHONY: all static_library shared_library client_static client_shared client_dynamic clean

all: client_static client_shared client_dynamic 

# Creating static library with .a extension
static_library:
	$(CXX) -c collatz.c 
	ar rcs libcollatz.a collatz.o

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
	objdump -d client_static > objcump_static.txt

objdump_shared: client_shared
	objdump -d client_shared > objcump_shared.txt

objdump_dynamic: client_dynamic
	objdump -d client_dynamic > objcump_dynamic.txt

clean:
	rm -f *.a *.so *.o 
	rm -f client_static client_shared client_dynamic
	rm -f client_static_objdump.txt client_shared_objdump.txt client_dynamic_objdump.txt
