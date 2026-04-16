CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fPIC -Iinclude 
LDFLAGS = -pthread 

all: crear_carpetas lib/libclaves.so lib/libproxyclaves.so bin/cliente bin/servidor_sock

crear_carpetas:
	mkdir -p obj
	mkdir -p lib
	mkdir -p bin

obj/claves.o: src/claves.c include/claves.h
	$(CC) $(CFLAGS) -c src/claves.c -o obj/claves.o

obj/app-cliente.o: src/app-cliente.c include/claves.h
	$(CC) $(CFLAGS) -c src/app-cliente.c -o obj/app-cliente.o

obj/proxy-sock.o: src/proxy-sock.c 
	$(CC) $(CFLAGS) -c src/proxy-sock.c -o obj/proxy-sock.o

obj/servidor-sock.o: src/servidor-sock.c 
	$(CC) $(CFLAGS) -c src/servidor-sock.c -o obj/servidor-sock.o

lib/libclaves.so: obj/claves.o
	$(CC) -shared -o lib/libclaves.so obj/claves.o $(LDFLAGS)

lib/libproxyclaves.so: obj/proxy-sock.o
	$(CC) -shared -o lib/libproxyclaves.so obj/proxy-sock.o $(LDFLAGS)

bin/cliente: obj/app-cliente.o lib/libproxyclaves.so
	$(CC) -o bin/cliente obj/app-cliente.o -Llib -lproxyclaves -Wl,-rpath,'$$ORIGIN/../lib' $(LDFLAGS)

bin/servidor_sock: obj/servidor-sock.o obj/claves.o
	$(CC) -o bin/servidor_sock obj/servidor-sock.o obj/claves.o $(LDFLAGS)

clean:
	rm -rf obj
	rm -rf lib
	rm -rf bin

.PHONY: all crear_carpetas clean