#include "database_server_library.h"
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

int init_server(struct sockaddr_in* server_addr) {
    Tables database;
    init_database(&database);
    WSADATA wsaData;
    SOCKET server_socket;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;  // Accept any incoming connection
    server_addr->sin_port = htons(SERVER_PORT); // Convert port to network byte order

    // Step 4: Bind the socket
    if (bind(server_socket, (struct sockaddr*)server_addr, sizeof(*server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();                // Cleanup WinSock resources
        return -1;
    }

    // Step 5: Listen for incoming connections
    if (listen(server_socket, 100) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();                // Cleanup WinSock resources
        return -1;
    }

    printf("Server is listening on port %d...\n", SERVER_PORT);
    return server_socket;
}

void* handle_client(void* client_args) {
    Client_args* args = (Client_args*)client_args;
    SOCKET client_socket = args->client_socket;
    Tables* database_ptr = args->database;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    int client_permission = 0;
    char name[255] ;
    char pass[255] ;
    int i = 100;
    add_attribute(database_ptr, USERS_TABLE, 1, USER_COMPUTER, &i);
    add_attribute(database_ptr, USERS_TABLE, 2, USER_COMPUTER, &i);

    // Receive and verify client name
    bytes_received = recv(client_socket, name, sizeof(name), 0);
    printf("name:%s\n", name);
    if (bytes_received <= 0) {
        printf("Failed to receive name or connection closed by client: %s\n", inet_ntoa(args->client_addr.sin_addr));
        closesocket(client_socket);
        return NULL;
    }

    bytes_received = recv(client_socket, pass, sizeof(pass), 0);
    if (bytes_received <= 0) {
        printf("Failed to receive password or connection closed by client: %s\n", inet_ntoa(args->client_addr.sin_addr));
        closesocket(client_socket);
        return NULL;
    }
    printf("pass:%s\n", pass);
    //if (is_safe_string(pass, 255) == E_UN_I) {
    //    printf("Unsafe input for password received.\n");
    //    closesocket(client_socket);
    //    return NULL;
    //}

    // Authenticate client and set permissions
    client_permission = checking_managers_permissions(database_ptr, name, pass);
    if (client_permission != E_NOT_F) {
        printf("Client %s has manager permissions.\n", inet_ntoa(args->client_addr.sin_addr));
        strcpy(buffer, "hello manager");
    }
    else {
        printf("Client %s has guest permissions.\n", inet_ntoa(args->client_addr.sin_addr));
        strcpy(buffer, "hello guest");
    }

    // Send initial permission-based greeting to the client
    if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
        printf("Failed to send greeting to client: %d\n", WSAGetLastError());
        closesocket(client_socket);
        return NULL;
    }

    // Main loop to handle client requests
    while (TRUE) {
        Server_response server_response = { 0 };
        Client_request client_request = { 0 };
        int res;

        // Receive a client request
        bytes_received = recv(client_socket, (char*)&client_request, sizeof(client_request), 0);
        if (bytes_received <= 0) {
            printf("Client %s disconnected or receive error: %d\n", inet_ntoa(args->client_addr.sin_addr), WSAGetLastError());
            break;
        }

        // Process the client request
        pthread_mutex_lock(&db_mutex);  // Lock the mutex before accessing the database
        res = run_cmd(database_ptr, client_request, &server_response, client_permission);
        pthread_mutex_unlock(&db_mutex);  // Unlock the mutex after accessing the database

        if (res == E_NOT_E || res == E_SUCCESS||res>0) {
            printf("return value form the function:%s\n", server_response.response);
            printf("Command executed successfully by client %s.\n", inet_ntoa(args->client_addr.sin_addr));
            strncpy(server_response.response, "Command execution success", sizeof(server_response.response) - 1);
        }
        else {
            printf("return value form the function:%s\n", server_response.response);
            printf("Command execution failed for client %s.\n", inet_ntoa(args->client_addr.sin_addr));
            strncpy(server_response.response, "Command execution failed", sizeof(server_response.response) - 1);
        }

        // Send response back to the client
        int bytes_sent = 0;
        while (bytes_sent < sizeof(server_response)) {
            int send_status = send(client_socket, (char*)&server_response + bytes_sent, sizeof(server_response) - bytes_sent, 0);
            if (send_status == SOCKET_ERROR) {
                printf("Failed to send response to client: %d\n", WSAGetLastError());
                break;
            }
            bytes_sent += send_status;
        }
    }

    closesocket(client_socket);
    return NULL;
}


int checking_managers_permissions(Tables *database,char* name, char* pass) {
    int manager_permission = E_NOT_F;
    for (int i = 0;i < database->database_settings.num_of_managers;i++) {
        if (strcmp(database->managers_table[i].manager_name, name) == 0 && strcmp(database->managers_table[i].manager_pass, pass) == 0) {
            manager_permission = database->managers_table[i].manager_permission;
        }
    }
    return manager_permission;
}

int run_cmd(Tables *database, Client_request client_request, Server_response *server_response, User_permission client_permission) {
    server_response->response_id = client_request.request_id;
    server_response->cmd = client_request.cmd;
    int res = -1;
    char res_c;
    int size = (database->database_settings.num_of_users > database->database_settings.num_of_users_data) ? database->database_settings.num_of_users : database->database_settings.num_of_users_data;
    
    int users_id[size];
    switch (client_request.cmd) {
        // Commands allowed for both guests and managers
    case CMD_PRINT_DATABASE:
        printf("table: %d\n", client_request.parameters.flag_parameters[TABLE_PLACE]);

        res= print_database(*database, client_request.parameters.flag_parameters[TABLE_PLACE]);
        printf("res = %d\n",res);
        sprintf(server_response->response, "%d", res);

        break;

    case CMD_PRINT_BY_ID:
        res = print_by_id(*database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.int_parameters[USER_ID_PLACE]);
        sprintf(server_response->response, "%d", res);
        
        break;

    case CMD_PRINT_PRIMARY_KEYS:
        print_primary_keys(*database, client_request.parameters.flag_parameters[TABLE_PLACE]);
        sprintf(server_response->response, "%d", res);
        break;

    case CMD_PRINT_UNIQUE_KEYS:
        print_uinque_keys(*database, client_request.parameters.flag_parameters[TABLE_PLACE]);
        sprintf(server_response->response, "%d", res);

        break;

    case CMD_SEARCH_BY_ATTRIBUTE:
        res = search_by_attribute(*database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.flag_parameters[1], client_request.parameters.string_parameters, users_id);
        sprintf(server_response->response, "%d", res);
        break;

    case CMD_RANGE_SEARCH_DATABASE:
        res = range_search_databse(*database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.int_parameters[0], client_request.parameters.int_parameters[1], users_id);
        sprintf(server_response->response, "%d", res);
        break;

    default:
        if (client_permission == PERMISSION_MANAGER) {
            // Commands only allowed for managers
            switch (client_request.cmd) {
            case CMD_INIT_DATABASE:
                res  = init_database(database);  
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_SAVE_DATABASE_SD:
                res = save_database_sd(database);
                sprintf(server_response->response, "%d", res);

                break;

            case CMD_LOAD_FROM_SD:
                res = load_from_sd(database);
                sprintf(server_response->response, "%d", res);

                break;

            case CMD_ADD_ROW:
                res = add_row(database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.flag_parameters[FLAG_PLACE]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_ADD_ATTRIBUTE:
                res = add_attribute(database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.int_parameters[USER_ID_PLACE], client_request.parameters.flag_parameters[ATTRIBUTE_PLACE], client_request.parameters.string_parameters);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_ATTRIBUTE_TYPE:
                res_c = attribute_type(client_request.parameters.flag_parameters[0]);
                sprintf(server_response->response, "%c", res);
                break;

            case CMD_SET_PRIMARY_KEYS:
                res = set_primary_keys(database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.flag_parameters[1]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_SET_UNIQUE_KEYS:
                res = set_uinque_keys(database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.flag_parameters[1]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_CHECK_PRIMARY:
                res = check_primary(*database, client_request.parameters.flag_parameters[0], client_request.parameters.flag_parameters[1]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_CHECK_UNIQUE:
                res = check_uinque(*database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.flag_parameters[1]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_REALLOC_DATABASE:
                res = realloc_database(database, client_request.parameters.flag_parameters[TABLE_PLACE], client_request.parameters.int_parameters[0]);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_CHECK_UNIQUENESS:
                res = check_uniqueness(database, client_request.parameters.flag_parameters[ATTRIBUTE_PLACE], client_request.parameters.string_parameters);
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_CLEAN_FILE:
                res = clean_file("file_path.txt");
                sprintf(server_response->response, "%d", res);
                break;

            case CMD_GET_LAST_ERROR:
                res = get_last_error("error_log.txt", NULL);
                sprintf(server_response->response, "%d", res);
                break; 

            default:
                printf("Unknown command or insufficient permissions: %d\n", client_request.cmd);
                break;
            }
        }
        else {
            printf("Permission denied: Client is not allowed to modify the database.\n");
        }
        break;
    }
    return res;

}


int is_safe_string(const char* input,int max_size) {
    // Check if string is too large
    if (strlen(input) >= max_size) {
        printf("Error: String exceeds maximum allowed size of %d characters.\n", max_size);
        return E_UN_I;  
    }
    // Check if string contains non-printable or unsafe characters
    for (int i = 0; i < strlen(input); i++) {
        if (!isprint(input[i])) {  // Non-printable characters
            printf("Error: String contains non-printable characters.\n");
            return E_UN_I;
        }
        if (input[i] == ',' || input[i] == ';' || input[i] == '.' || input[i] == '|' || input[i] == '&') {
            printf("Error: String contains unsafe characters.\n");
            return E_UN_I ;
        }
    }

    return E_NOT_E;  
}