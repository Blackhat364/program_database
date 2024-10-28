#include "database_server_library.h"



int main() {
    struct sockaddr_in server_addr, client_addr;
    SOCKET server_socket, client_socket;
    int client_addr_len = sizeof(client_addr);
    Tables database;
  
    if ((init_database(&database)) != E_NOT_E) {
        printf("database initialization failed.\n");
        return 1;
    }
    //printf("error:%d\n", add_row(&database, USERS_TABLE, LEVEL0));
    //printf("error:%d\n", add_row(&database, USERS_TABLE, LEVEL0));
    //print_database(database, USERS_TABLE);
    //printf("error:%d\n", add_row(&database, USERS_TABLE, LEVEL0));
    //save_database_sd(&database);
    //printf("error:%d\n", add_row(&database, USERS_TABLE, LEVEL0));
    //printf("Loaded user_id_list: %c\n", database.database_settings.user_id_list[3]);
    //save_database_sd(&database);

    //printf("1 %c\n", database.database_settings.user_id_list[0]);
    if ((load_from_sd(&database)) != E_NOT_E) {
        printf("database load from sd failed.\n");
    }
    printf("Loaded num_of_users: %d\n", database.database_settings.num_of_users);
    printf("Loaded user_id_list: %d\n", sizeof(database.database_settings.user_id_list));
    print_database(database, ALLTABLES);



    server_socket = init_server(&server_addr);
    if (server_socket == -1) {
        printf("Server initialization failed.\n");
        return 1;
    }
    

    /*for (int i = 0;i < 10;i++) {
    add_row(&database, USERS_TABLE, LEVEL0);
    }*/
    //add_row(database_ptr, USERS_TABLE, LEVEL0);
    while (TRUE) {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue; 
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


        // Create a new thread for the client
        pthread_t client_thread;

        Client_args client_args = {
            .client_addr = client_addr,
            .client_socket = client_socket,
            .database = &database
        };
        if (pthread_create(&client_thread, NULL, handle_client, &client_args) != 0) {
            printf("Thread creation failed.\n");
            closesocket(client_socket);
        }

        // Detach the thread to allow automatic resource cleanup after it finishes
        pthread_detach(client_thread);
    }

    // Close the server socket
    closesocket(server_socket);
    WSACleanup();

    return 0;
}