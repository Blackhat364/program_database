#ifndef database_library_h
#define database_library_h

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h> 
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

typedef unsigned int Flag;
#define DEFAULT_SIZE 100
#define USERS_TABLE 0
#define USERS_DATA_TABLE 1
#define ALLTABLES 2
//system Flags
#define LEVEL0 0b0000
#define LEVEL1 0b0001
#define LEVEL2 0b0010
#define LEVEL3 0b0100

//

#define USER_ID 0b0001
#define USER_NAME 0b0010//
#define USER_PASS 0b0100//
#define USER_EMAIL 0b1000//
#define USER_PHON 0b00010000//
#define USER_PERMISSION 0b00100000//permission
#define USER_COMPUTER 0b01000000//computer

//permissions five level or not but now five
#define ADMIN_PERMISSION 5



#define DATA_ID 0b0010
#define DATA_PATH 0b0100//
#define COMPUTER_ID 0b1000//
#define DATA_SIZE 0b00010000//
#define DATA_TYPE 0b00100000//

#define NUM_OF_MANAGERS 5
#define DEFAULT_INT -100
#define DEFAULT_STRING  "NULL"
#define DEFAULT_CHAR '~'



typedef int* Hash_table_type;


typedef struct {
    char error_time[20];        // Time as a string (formatted as YYYY-MM-DD HH:MM:SS)
    int error;                  // Error code
    char error_function[50];    // Function name as a string
    char error_message[20];     // Error message
} Error_struct;

typedef struct
{
    int hash_table_size;
    int number_of_table;
}Hash_table_setting;


typedef struct
{
    int user_id;
    char user_name[255];
    char user_pass[255];
    char user_email[255];
    char user_phon[11];
    int user_permission;
    int user_computer;
}Users;


typedef struct
{
    int user_id;
    int data_id;
    char data_path[200];
    int computer_id;
    int data_type;
    int data_size;

}Users_data;

typedef struct {
    int manager_id;
    char manager_name[255];
    char manager_pass[255];
    char manager_email[255];
    char manager_phon[11];
    int manager_permission;
}Managers;

typedef struct
{
    char log_erros_files_path[200];
    char database_save_file_path[200];
    char database_settings_file_path[200];
    int users_table_size;
    int users_data_table_size;
    char* user_id_list;
    char* user_data_id_list;
    int num_of_users;
    int num_of_users_data;
    int num_of_managers;
}Database_settings;

typedef struct
{
    Hash_table_type* hash_tables;
    Users* users;
    Users_data* users_data;
    Managers managers_table[NUM_OF_MANAGERS];
    Flag user_primary_keys;
    Flag user_data_primary_keys;
    Flag user_uinque_keys;
    Flag user_data_uinque_keys;
    Database_settings database_settings;
    Hash_table_setting  hash_table_setting;
    
}Tables;



int init_database(Tables *database); // init database 
int save_database_sd(Tables *database);
int load_from_sd(Tables* database);
int add_row(Tables *database,Flag table,Flag flag, ...);//need to add more feture but work
int add_attribute(Tables* database, Flag table,int id, Flag add_attribute, void* data); //work
char attribute_type(Flag attribute);
int search_by_attribute(Tables database, Flag table,Flag attribute, void* data,int * users_id);//if scssus return the user_id
int range_search_databse(Tables database, Flag table, int start, int end, int* users_id);
int handle_error(int error_code, const char* function, const char* message);
void free_database(Tables *database);//wrok
int print_database(Tables database, Flag table);//work
int print_by_id(Tables database, Flag table, int id);//work
int get_primary_id(Tables database,Flag table);
int set_primary_keys(Tables* database, Flag table, Flag attribute);
int set_uinque_keys(Tables* database, Flag table, Flag attribute);
int print_primary_keys(Tables database, Flag table);
int print_uinque_keys(Tables database, Flag table);
int check_primary(Tables database, Flag table, Flag attribute);
int check_uinque(Tables database, Flag table, Flag attribute);
int realloc_database(Tables* database, Flag table, int size);
int check_uniqueness(Tables* database, Flag attribute, char* data);
int clean_file(const char* file_path);
int get_last_error(const char* file_path, Error_struct* last_error);
int add_manager(Tables* database, Managers manager);
int init_managers(Tables* database);

#endif