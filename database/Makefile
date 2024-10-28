# -*- Makefile -*-

CC=gcc
CFLAGS=-Wall -std=c99
LDFLAGS=-lm -lws2_32 


all: database

database: database.o database_library.o hash_library.o
	$(CC) $(LDFLAGS) database.o database_library.o hash_library.o -o database

database.o: database.c database_library.h 
	$(CC) $(CFLAGS) -c database.c

database_library.o: database_library.c database_library.h
	$(CC) $(CFLAGS) -c database_library.c

hash_library.o: hash_library.c hash_library.h
	$(CC) $(CFLAGS) -c hash_library.c

clean_o:
	rm -f *.o 

clean_exe:
	rm -f *.exe
