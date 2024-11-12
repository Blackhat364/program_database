#ifndef hash_library_h
#define hash_library_h
#include "database_library.h"


#define HASH_TABLE_NAME 0
#define HASH_TABLE_PASS 1
#define HASH_TABLE_EMAIL 2
#define HASH_TABLE_PHON 3

//flags
#define FLAG_BIT    0b11111111000000000000000000000000
#define CHANGES_BIT 0b00000000111111110000000000000000
#define KEY_BIT     0b00000000000000001111111111111111
#define OCCUIPIED_FLAG 0b00000001000000000000000000000000

#define NUM_OF_SHIFTING_FLAG_BIT 24
#define NUM_OF_SHIFTING_CHANGES_BIT 16



#define MAX_MOVE 255
#define MAX_SIZE_HASH_KEY_SIZE 65535




void free_hash_table(Tables* database);


int init_hash_table(Tables* database);

unsigned int hash_function(char* input);

int pot_in_hash_table(Tables* database, Flag level,...);

int search_in_hash_table(Tables* database, Flag table, char* string);

int print_hash_tabel(Tables* database, Flag table);


int SHIFTING_RIGHT(int * pointer,int num_of_shifting);

#endif	
