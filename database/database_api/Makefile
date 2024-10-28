CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lm -lws2_32 -lpthreadGC2 -mconsole

all: database_server database_client 

# Link the object files into the final executable
database_server:database_library.o hash_library.o database_server.o database_server_library.o 
	$(CC) $(CFLAGS) database_server.o database_server_library.o database_library.o hash_library.o $(LDFLAGS) -o database_server

# Compile the source files into object files
database_server.o: database_server.c database_server_library.h 
	$(CC) $(CFLAGS) -c database_server.c

database_server_library.o: database_server_library.c database_server_library.h
	$(CC) $(CFLAGS) -c database_server_library.c

#database librarys

database_library.o: database_library.c database_library.h
	$(CC) $(CFLAGS) -c database_library.c

hash_library.o: hash_library.c hash_library.h
	$(CC) $(CFLAGS) -c hash_library.c

database_client: database_client.o database_api.o
		$(CC) $(CFLAGS) database_client.o database_api.o $(LDFLAGS) -o database_client

database_client.o: database_client.c database_api.h 
	$(CC) $(CFLAGS) -c database_client.c

database_api.o : database_api.c database_api.h 
	$(CC) $(CFLAGS) -c database_api.c



run_d:
	./database_server.exe

run_c:
	./database_client.exe
# Clean up object files

clean_all: clean_o clean_exe

clean_o:
	rm -f *.o 

# Clean up executable files
clean_exe:
	rm -f *.exe
