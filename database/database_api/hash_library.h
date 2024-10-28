#ifndef hash_library_h
#define hash_library_h
#include "database_library.h"


#define HASH_TABLE_NAME 0
#define HASH_TABLE_PASS 1
#define HASH_TABLE_EMAIL 2
#define HASH_TABLE_PHON 3




void free_hash_table(Tables* database);


int init_hash_table(Tables* database);

unsigned int hash_function(char* input);

int pot_in_hash_table(Tables* database, Flag level,...);

int search_in_hash_table(Tables* database, Flag table, char* string);

int print_hash_tabel(Tables* database, Flag table);
#endif	
