#ifndef database_client_library_h
#define database_client_library_h
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
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "errors_file.h"

#define DEFAULT_SIZE 100
#define DEFAULT_INT -100
#define DEFAULT_STRING  "NULL"
#define DEFAULT_CHAR  '~'
#define DEFAULT_FLAG -100
#define NUM_OF_PAEAMTERS 5

#define DEFAULT_SIZE 100
#define USERS_TABLE 0
#define USERS_DATA_TABLE 1
#define ALLTABLES 2

#define LEVEL0 0b0000
#define LEVEL1 0b0001
#define LEVEL2 0b0010
#define LEVEL3 0b0100

#define USER_ID 0b0001
#define USER_NAME 0b0010//
#define USER_PASS 0b0100//
#define USER_EMAIL 0b1000//
#define USER_PHON 0b00010000//
#define USER_PERMISSION 0b00100000//permission
#define USER_COMPUTER 0b01000000//computer

#define DATA_ID 0b0010
#define DATA_PATH 0b0100//
#define COMPUTER_ID 0b1000//
#define DATA_SIZE 0b00010000//
#define DATA_TYPE 0b00100000//

//attrbute
#define USER_ID 0b0001
#define USER_NAME 0b0010//
#define USER_PASS 0b0100//
#define USER_EMAIL 0b1000//
#define USER_PHON 0b00010000//
#define USER_PERMISSION 0b00100000//permission
#define USER_COMPUTER 0b01000000//computer

#define DATA_ID 0b0010
#define DATA_PATH 0b0100//
#define COMPUTER_ID 0b1000//
#define DATA_SIZE 0b00010000//
#define DATA_TYPE 0b00100000//



//constns place on the list of paramters
#define TABLE_PLACE 0 // in flag Paramters array 
#define ATTRIBUTE_PLACE 1//in flag Paramters array
#define FLAG_PLACE 2

#define USER_ID_PLACE 0 // in int  Paramters array
#define USER_DATA_ID_PLACE 0 // in int  Paramters array
#define START_RANGE_PLACE 1
#define END_RANGE_PLACE 2

#define DATA_POINTER 
#define OUTPUT_LIST

typedef unsigned int Flag;
#define NUM_OF_COMMAND 22
typedef enum {
    CMD_INIT_DATABASE = 1,        // Initialize the database
    CMD_SAVE_DATABASE_SD,         // Save database to storage
    CMD_LOAD_FROM_SD,             // Load database from storage
    CMD_ADD_ROW,                  // Add a row to a table
    CMD_ADD_ATTRIBUTE,            // Add an attribute to a row
    CMD_ATTRIBUTE_TYPE,           // Get the attribute type
    CMD_SEARCH_BY_ATTRIBUTE,      // Search the database by attribute
    CMD_RANGE_SEARCH_DATABASE,    // Perform a range search on the database
    CMD_HANDLE_ERROR,             // Handle an error
    CMD_FREE_DATABASE,            // Free the allocated memory for the database
    CMD_PRINT_DATABASE,           // Print the entire database
    CMD_PRINT_BY_ID,              // Print a row by ID
    CMD_SET_PRIMARY_KEYS,         // Set the primary keys
    CMD_SET_UNIQUE_KEYS,          // Set unique keys for the database
    CMD_PRINT_PRIMARY_KEYS,       // Print primary keys of a table
    CMD_PRINT_UNIQUE_KEYS,        // Print unique keys of a table
    CMD_CHECK_PRIMARY,            // Check if a primary key exists
    CMD_CHECK_UNIQUE,             // Check if a unique key exists
    CMD_REALLOC_DATABASE,         // Reallocate the database memory
    CMD_CHECK_UNIQUENESS,         // Check uniqueness of an attribute
    CMD_CLEAN_FILE,               // Clean or clear a file
    CMD_GET_LAST_ERROR            // Retrieve the last logged error
} Command_code;

//typedef struct
//{
//    char response
//}Response;

typedef struct
{
    char string_parameters[255];
    char char_parameters[NUM_OF_PAEAMTERS];
    int int_parameters[NUM_OF_PAEAMTERS];
    Flag flag_parameters[NUM_OF_PAEAMTERS];
}Parameters;

typedef struct {
    int request_id;
    Command_code cmd;
    Parameters parameters;
} Client_request;


typedef struct
{
    int response_id;
    Command_code cmd;
    char response[255];
}Server_response;

SOCKET init_client(struct sockaddr_in* server_addr, char* name, char* pass, Flag flag);
Server_response send_request(SOCKET server_socket, Client_request requset);
int set_request(Client_request* client_request,Command_code cmd, Parameters parameters);
int init_array(void* arr, char type, int length);
int init_parameters(Parameters* parameters);
int get_request_id();
int set_parameters(Parameters* parameters, int num_args, ...);
int print_parameters(Parameters parameters);
int print_request(Client_request client_request);
int set_cmd_from_console(Parameters* parameters, Client_request* client_request);
int menus(int* choice, Flag level);
char attribute_type(Flag attribute);
int is_safe_string(const char* input, int max_size);

#endif