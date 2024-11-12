#include "database_api.h"
#define SERVER_IP "127.0.0.1"  
#define SERVER_PORT 2000       

char* requests_id;

SOCKET init_client(struct sockaddr_in* server_addr,char *server_ip,int server_port, char* name1, char* pass1, Flag flag) {
    WSADATA wsaData;
    SOCKET clientSocket;
    char buffer[200];
    int result;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Configure server address
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = inet_addr(server_ip);  // Convert IP address to network byte order
    server_addr->sin_port = htons(server_port);           // Convert port number to network byte order

    // Connect to the server
    result = connect(clientSocket, (struct sockaddr*)server_addr, sizeof(*server_addr));
    if (result == SOCKET_ERROR) {
        printf("Connection to server failed. Error Code: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    char name[255];
    char pass[255];
    // Send name and password to the server
    if (flag & LEVEL1 == LEVEL1) {
        strncpy(name, name1, 255);
        strncpy(pass, pass1, 255);
    }
    else {
        strcpy(name, "dror_admin");
        strcpy(pass, "d1r2o3r4");
    }
    
    send(clientSocket, name, 255, 0);
    send(clientSocket, pass, 255, 0);

    // Receive server response
    result = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Reserve space for null-terminator
    if (result > 0) {
        buffer[result] = '\0';  // Null-terminate the received data
        printf("Server response: %s\n", buffer);
    }
    else if (result == 0) {
        printf("Server closed the connection.\n");
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    else {
        printf("Failed to receive server response. Error Code: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Allocate and initialize request_id
    requests_id = (char*)malloc(sizeof(char) * DEFAULT_SIZE);
    if (!requests_id) {
        printf("Memory allocation for requests_id failed.\n");
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    memset(requests_id, '0', DEFAULT_SIZE); // Initialize request_id with '0'

    return clientSocket;
}
Server_response send_request(SOCKET server_socket, Client_request request) {
    Server_response server_response;
    int send_status, recv_status;
    int bytes_sent = 0, bytes_received = 0;
    //char buffer[1000];
    // Send the request to the server
    //printf("table in send function : %d\n", request.parameters.flag_parameters[TABLE_PLACE]);
    while (bytes_sent < sizeof(request)) {
        send_status = send(server_socket, (char*)&request + bytes_sent, sizeof(request) - bytes_sent, 0);
        if (send_status == SOCKET_ERROR) {
            printf("Error sending request to server: %d\n", WSAGetLastError());
            return server_response;  // return an empty/invalid response
        }
        bytes_sent += send_status;  // Track the total bytes sent
    }

    // Receive the response from the server
    //while (bytes_received < sizeof(server_response)) {
    //    recv_status = recv(server_socket, (char*)&server_response, sizeof(server_response), 0);
    //    if (recv_status == SOCKET_ERROR) {
    //        printf("Error receiving response from server: %d\n", WSAGetLastError());
    //        return server_response;  
    //    }
    //    bytes_received += recv_status;  
    //}
     bytes_received = recv(server_socket, (char*)&server_response, sizeof(server_response), 0);
    if (bytes_received > 0) {
        printf("Cmd in server: %s\n", server_response.response);
    }
    else if (bytes_received == 0) {
        printf("Connection closed by server.\n");
    }
    else {
        printf("Receive failed. Error Code: %d\n", WSAGetLastError());
    }




    return server_response;
}

int set_request(Client_request* client_request, Command_code cmd, Parameters parameters) {
    // Step one: set command
    client_request->cmd = cmd;
    strcpy(client_request->parameters.string_parameters, parameters.string_parameters);
    for (int i = 0; i < NUM_OF_PAEAMTERS; i++) {
        client_request->parameters.int_parameters[i] = parameters.int_parameters[i];

        client_request->parameters.flag_parameters[i] = parameters.flag_parameters[i];
        
        client_request->parameters.char_parameters[i] = parameters.char_parameters[i];
        
    }

    // Step three: set request ID
    client_request->request_id = get_request_id();

    return E_NOT_E;
}

int init_array(void* arr, char type, int length) {
    if (length <= 0) {
        printf("Invalid array length.\n");
        return E_EN_IV_IN;
    }

    int i;
    switch (type) {
    case 'i':
        for (i = 0; i < length; i++) {
            ((int*)arr)[i] = DEFAULT_INT; // Explicitly initialize each int element
        }
        break;
    case 'c':
        memset(arr, DEFAULT_CHAR, length * sizeof(char)); // This works for char arrays
        break;
    case 'f':
        for (i = 0; i < length; i++) {
            ((Flag*)arr)[i] = DEFAULT_FLAG;  // Initialize Flag struct array
        }
        break;
    default:
        printf("Invalid type!\n");
        return E_EN_IV_IN;
    }

    return E_NOT_E;
}
int init_parameters(Parameters* parameters) {
    int res = E_NOT_E;
    strcpy(parameters->string_parameters,"NULL");
    res = init_array(parameters->int_parameters, 'i', NUM_OF_PAEAMTERS);
    if (res == E_EN_IV_IN) {
        return res;
    }

    res = init_array(parameters->flag_parameters, 'f', NUM_OF_PAEAMTERS); // Use 'f' for flags
    if (res == E_EN_IV_IN) {
        return res;
    }

    res = init_array(parameters->char_parameters, 'c', NUM_OF_PAEAMTERS);
    if (res == E_EN_IV_IN) {
        return res;
    }

    return res;
}


int get_request_id(){
    for (int i = 0; i < DEFAULT_SIZE; i++)
    {
        if (requests_id[i] == '0') {
            requests_id[i] = '1';
            return i;
        }
    }
    return E_R_O_S;
}

int set_parameters(Parameters* parameters, int num_args, ...) {
    va_list args;
    va_start(args, num_args%10);

    va_list args_copy;
    va_copy(args_copy, args);

    // Loop through all provided arguments
    if (num_args/10 == 1) {
        for (int i = 0; i < num_args%10; i++) {//for for the int 
            int type = va_arg(args, int);
            switch (type) {
            case USER_ID_PLACE:
                parameters->int_parameters[USER_ID_PLACE] = va_arg(args, int);
                break;

            case START_RANGE_PLACE:
                parameters->int_parameters[START_RANGE_PLACE] = va_arg(args, int);
                break;

            case END_RANGE_PLACE:
                parameters->int_parameters[END_RANGE_PLACE] = va_arg(args, int);
                break;

            default:
                printf("Error: Unknown parameter type %d\n", type);
                va_end(args);
                return -1;
            }
        }
    }
    else if (num_args/10 == 2) {
        for (int i = 0;i < num_args% 10;i++) {//for for flag type i do this beaces flag is typedef of int
            int type = va_arg(args_copy, int);  // Retrieve the type 
            switch (type) {
            case TABLE_PLACE:
                parameters->flag_parameters[TABLE_PLACE] = va_arg(args_copy, Flag);
                break;

            case ATTRIBUTE_PLACE:
                parameters->flag_parameters[ATTRIBUTE_PLACE] = va_arg(args_copy, Flag);
                break;
            case FLAG_PLACE:
                parameters->flag_parameters[FLAG_PLACE] = va_arg(args_copy, Flag);
                break;
            default:
                printf("Error: Unknown parameter flag type %d\n", type);
                va_end(args);
                va_end(args_copy);
                return -1;
            }
        }
    }
    va_end(args_copy);
    va_end(args);
    return E_NOT_E;
}

void free_api() {
    free(requests_id);
}


int print_parameters(Parameters parameters) {
    int i;
    // Print string parameters
    printf("\tString Parameters: %s\n", parameters.string_parameters);

    // Print char parameters
    printf("\tChar Parameters:\n");
    for (i = 0; i < NUM_OF_PAEAMTERS; i++) {
        printf("\t  [%d] % c\n", i, parameters.char_parameters[i]);
    }

    // Print int parameters
    printf("\tInt Parameters:\n");
    for (i = 0; i < NUM_OF_PAEAMTERS; i++) {
        printf("\t  [%d] %d\n", i, parameters.int_parameters[i]);
    }

    // Print flag parameters
    printf("\tFlag Parameters:\n");
    for (i = 0; i < NUM_OF_PAEAMTERS; i++) {
        printf("\t  [%d] %d\n", i, parameters.flag_parameters[i]);
    }
    return E_NOT_E;
}

int print_request(Client_request client_request) {
    printf("   ----request-----\n");
    printf("\trequest id:%d\n", client_request.request_id);
    printf("\trequest cmd:%d\n",client_request.cmd);
    print_parameters(client_request.parameters);
    printf("   ------end-------\n");

    return E_NOT_E;

}

int set_cmd_from_console(Parameters* parameters, Client_request* client_request){
    init_parameters(parameters);  // Initialize parameters
    int choice = 0;
    menus(&choice, LEVEL0);
    Command_code cmd = choice;
    int input_int = -100;
    char input_char = '~';
    char input_string[255] = "NULL";
    printf("enter table:");
    scanf("%d",&input_int);
    set_parameters(parameters, 21, TABLE_PLACE, input_int);
    while (getchar() != '\n');
    printf("enter attrubute:");
    scanf("%d", &input_int);
    set_parameters(parameters, 21, ATTRIBUTE_PLACE, input_int);
    while (getchar() != '\n');
    printf("enter user /user_data id:");
    scanf("%d", &input_int);
    set_parameters(parameters, 11, USER_ID_PLACE, input_int);
    while (getchar() != '\n');
    printf("enter level/flag:");
    scanf("%d", &input_int);
    set_parameters(parameters, 21, FLAG_PLACE, input_int);
    switch (cmd)
    {
    case CMD_RANGE_SEARCH_DATABASE:
        while (getchar() != '\n');
        printf("start:");
        scanf("%d", &input_int);
        set_parameters(parameters, 11, START_RANGE_PLACE, input_int);
        while (getchar() != '\n');
        printf("end:");
        scanf("%d", &input_int);
        set_parameters(parameters, 11, END_RANGE_PLACE, input_int);
        break;
    case CMD_SEARCH_BY_ATTRIBUTE:
    case CMD_CHECK_UNIQUENESS:
    case CMD_ADD_ATTRIBUTE :
        while (getchar() != '\n');
        void* ptr;
        input_char = attribute_type(parameters->flag_parameters[ATTRIBUTE_PLACE]);
        printf("enter data:");
        switch (input_char)
        {
        case 'i':
            scanf("%s", input_string);
            strcpy(parameters->string_parameters, input_string);
            break;
        case 's':
            scanf("%s", input_string);
            strcpy(parameters->string_parameters, input_string);
            break;
        default:
            printf("you enter worng attribute!!\n");
            break;
        }
        
    default:
        break;
    }
    return cmd;
}


int menus(int* choice, Flag level) {
    switch (level)
    {
    case LEVEL0:
        while (1) {
            printf("Commands:\n"
                "\t1. CMD_INIT_DATABASE\n"
                "\t2. CMD_SAVE_DATABASE_SD\n"
                "\t3. CMD_LOAD_FROM_SD\n"
                "\t4. CMD_ADD_ROW\n"
                "\t5. CMD_ADD_ATTRIBUTE\n"
                "\t6. CMD_ATTRIBUTE_TYPE\n"
                "\t7. CMD_SEARCH_BY_ATTRIBUTE\n"
                "\t8. CMD_RANGE_SEARCH_DATABASE\n"
                "\t9. CMD_HANDLE_ERROR\n"
                "\t10. CMD_FREE_DATABASE\n"
                "\t11. CMD_PRINT_DATABASE\n"
                "\t12. CMD_PRINT_BY_ID\n"
                "\t13. CMD_SET_PRIMARY_KEYS\n"
                "\t14. CMD_SET_UNIQUE_KEYS\n"
                "\t15. CMD_PRINT_PRIMARY_KEYS\n"
                "\t16. CMD_PRINT_UNIQUE_KEYS\n"
                "\t17. CMD_CHECK_PRIMARY\n"
                "\t18. CMD_CHECK_UNIQUE\n"
                "\t19. CMD_REALLOC_DATABASE\n"
                "\t20. CMD_CHECK_UNIQUENESS\n"
                "\t21. CMD_CLEAN_FILE\n"
                "\t22. CMD_GET_LAST_ERROR\n");
            printf("Enter command (1 - %d): ", NUM_OF_COMMAND);

            // Clear the input buffer
            int result = scanf("%d", choice);

            if (result != 1) {
                printf("Error: Invalid input. Please enter a number.\n");

                // Clear the input buffer
                while (getchar() != '\n');
                continue;
            }


            if (*choice >= 1 && *choice <= NUM_OF_COMMAND) {
                printf("You selected command: %d\n", *choice);
                break;
            }
            else {
                printf("Error: Command is out of range (1 - %d).\n", NUM_OF_COMMAND);
            }
        }
        break;
    default:
        printf("Error: Invalid menu.\n");
        return E_EN_IV_IN;
        break;
    }
    return E_NOT_E;
}

char attribute_type(Flag attribute) {
    switch (attribute) {
    case USER_NAME:
        return 's';
        break;
    case USER_PASS:
        return 's';
        break;
    case USER_EMAIL:
        return 's';
        break;
    case USER_PHON:
        return 's';
        break;
    case USER_PERMISSION:
        return 'i';
        break;
    case USER_COMPUTER:
        return 'i';
        break;
    case USER_ID:
        return 'i';
        break;
    }
    switch (attribute) {
    case DATA_PATH:
        return 's';
        break;
    case COMPUTER_ID:
        return 'i';
        break;
    case DATA_SIZE:
        return 'i';
        break;
    case DATA_TYPE:
        return 'i';
        break;

    default:
        break;

    }
    return '3';
}


int is_safe_string(const char* input, int max_size) {
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
        if (input[i] == ';' || input[i] == '.' || input[i] == '|' || input[i] == '&') {
            printf("Error: String contains unsafe characters.\n");
            return E_UN_I;
        }
    }

    return E_NOT_E;
}
