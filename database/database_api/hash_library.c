#include "hash_library.h"
#include "errors_file.h"


unsigned int hash_function(char* input) {
    unsigned int key = 0;
    unsigned int prime = 31; 
    size_t length = strlen(input);

    for (size_t i = 0; i < length; i++) {
        if (input[i] == ' ') {
            continue;
        }
        key = key * prime + input[i];
    }

    return key % DEFAULT_SIZE; 
}
void free_hash_table(Tables* database) {
    for (int i = 0; i < (database->hash_table_setting.number_of_table); i++)
    {
        free(database->hash_tables[i]);
    }
    free(database->hash_tables);
}

int init_hash_table(Tables* database) {
    database->hash_table_setting.number_of_table = 4;
    database->hash_tables = (Hash_table_type)malloc(sizeof(Hash_table_type)* database->hash_table_setting.number_of_table);
    if (database->hash_tables == NULL) {
        printf("not can malloc memory error\n");
        return E_MAM_E;
    }
    for (int i = 0;i < database->hash_table_setting.number_of_table;i++) {
        database->hash_tables[i] = (int)malloc(sizeof(int* )*DEFAULT_SIZE);
        if (database->hash_tables[i] == NULL)
        {
            printf("not can malloc memory error\n");
            return E_MAM_E;
        }
    }
    for (int i = 0;i <database->hash_table_setting.number_of_table;i++) {
        for (int j = 0;j < DEFAULT_SIZE;j++) {
            database->hash_tables[i][j] = DEFAULT_INT;
        }
    }
    database->hash_table_setting.hash_table_size = DEFAULT_SIZE;
    return E_NOT_E;

}

int pot_in_hash_table(Tables* database,Flag table,...){
    va_list args;
    va_start(args, 1);
    if (table == HASH_TABLE_NAME) {
        
        Users user = va_arg(args, Users);
        if (strcmp(user.user_name, DEFAULT_STRING) != 0) {
            int index = hash_function(user.user_name);
            database->hash_tables[HASH_TABLE_NAME][index] = user.user_id;
        }
        else {
            return -8;
        }
    }
    else if(table == HASH_TABLE_PASS){
        Users user = va_arg(args, Users);
        
        if (strcmp(user.user_pass, DEFAULT_STRING) != 0) {
            int index = hash_function(user.user_pass);
            database->hash_tables[HASH_TABLE_PASS][index] = user.user_id;
        }
        else {
            return -8;
        }
    }
    else if (table == HASH_TABLE_EMAIL) {
        Users user = va_arg(args, Users);

        if (strcmp(user.user_email, DEFAULT_STRING) != 0) {
            int index = hash_function(user.user_email);
            database->hash_tables[HASH_TABLE_EMAIL][index] = user.user_id;
        }
        else {
            return -8;
        }
    }
    else if (table == HASH_TABLE_PHON) {
        Users user = va_arg(args, Users);

        if (strcmp(user.user_phon, DEFAULT_STRING) != 0) {
            int index = hash_function(user.user_phon);
            database->hash_tables[HASH_TABLE_PHON][index] = user.user_id;
        }
        else {
            return -8;
        }
    }
    
        
   
    va_end(args);
    return E_NOT_E;
}
int print_hash_tabel(Tables* database,Flag table) {
    printf("   -------hash_tabel number : %d --\n\n",table);
    for (int i = 0; i < (database->hash_table_setting.hash_table_size);i++) {
        if (database->hash_tables[table][i] != DEFAULT_INT) {
            printf("   index  = %d ,user_id : %d\n",i, database->hash_tables[table][i]);
        }

    }
    printf("\n   ----------end------\n");
    return E_NOT_E;

}

int get_key(char* string) {
    int key = hash_function(string);
    return key;
}

int search_in_hash_table(Tables* database,Flag table,char * string) {
    int key = hash_function(string);
    if (database->hash_tables[table][key] != DEFAULT_INT) {
        return database->hash_tables[table][key];
    }
    else {
        return E_NOT_F;
    }
    

}

int realloc_hash_table(Tables* database, Flag flag, int size) {
    if (flag == 0) {
        for (int i = 0; i < database->hash_table_setting.number_of_table; i++) {
            Hash_table_type* temp = realloc(database->hash_tables[i], sizeof(*(database->hash_tables[i])) * size);

            if (temp == NULL) {
                fprintf(stderr, "Failed to realloc memory for hash table %d\n", i);
                return E_MAM_E;  // Memory allocation error
            }
            else {
                database->hash_tables[i] = temp; 
                database->hash_table_setting.number_of_table += size;
            }
        }
    }
    return E_NOT_E;
}
