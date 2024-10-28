#include "database_library.h"
#include "errors_file.h"
#include "hash_library.h"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <ctype.h>  


#define SERVER_PORT 2000
#define SERVER_IP "10.0.0.3"
#define BUFFER_SIZE 1024
#define SAFE_STR_SIZE 255
#define NUM_OF_PAEAMTERS 5

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
//flag



typedef enum {
    PERMISSION_GUEST = 4,     // Can only view the database
    PERMISSION_MANAGER    // Can modify the database
} User_permission;

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
typedef struct
{
	struct sockaddr_in client_addr;
	SOCKET client_socket;
	Tables* database;
}Client_args;

int init_server(struct sockaddr_in* server_addr);
void* handle_client(void* Client_args);
int checking_managers_permissions(Tables* database, char* name, char* pass);
int run_cmd(Tables* database, Client_request client_requset, Server_response* server_response, User_permission client_permission);
int is_safe_string(const char* input,int max_size);

