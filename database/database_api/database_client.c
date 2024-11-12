#include "database_api.h"


int main(int argc, char** argv) {
	char server_ip[] = "127.0.0.1"; 
	int server_port = 2000;

	if (argc > 1) {
		strncpy(server_ip, argv[1], sizeof(server_ip) - 1);  
		server_ip[sizeof(server_ip) - 1] = '\0';  
		server_port = atoi(argv[2]);  
	}


	struct sockaddr_in server_addr;
	SOCKET server_socket = init_client(&server_addr,server_ip,server_port, "dror_admin","d1r2o3r4", LEVEL1);
	while (server_socket == INVALID_SOCKET) {
		printf("client initialization failed!\n");
		Sleep(3000);
		server_socket = init_client(&server_addr, server_ip, server_port, ,NULL,NULL,LEVEL0);
	}
	
	bool run = TRUE;
	int choich = 0;
	char choich_s[255];
	while (run) {
		Parameters parameters;
		Server_response server_response;
		Client_request client_request;
		Command_code cmd;
		cmd  = set_cmd_from_console(&parameters,&client_request);

		set_request(&client_request, cmd, parameters);
		print_request(client_request);
		server_response = send_request(server_socket, client_request);
		printf("response:%s\n", server_response.response);
		
		
		
	}

	closesocket(server_socket);
	WSACleanup();
	return 0;
}
