#include "database_api.h"


int main() {
	
	struct sockaddr_in server_addr;
	SOCKET server_socket = init_client(&server_addr, "dror_admin","d1r2o3r4", LEVEL1);
	while (server_socket == INVALID_SOCKET) {
		printf("client initialization failed!\n");
		Sleep(3000);
		server_socket = init_client(&server_addr,NULL,NULL,LEVEL0);
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