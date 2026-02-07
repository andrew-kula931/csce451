#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

/**
 * Provides a separate function to handle interactions with the client
 * @param socketD The socket id
 * @return 0 on successful completion
 */
int handle_server(int socketD) {
	while (1 == 1) {
		char strData[255];
		int bytesReceived = recv(socketD, strData, sizeof(strData), 0);
		if (bytesReceived > 0) {
			// The first part is the server directory prompt (server/root>)	
			printf("%s", strData);

			char input[255];		
			fgets(input, sizeof(input), stdin);
			if (strcmp(input, "quit\n") == 0 || strcmp(input, "q\n") == 0) break;

			send(socketD, input, sizeof(input), 0);

			// Currently two recieves in order to handle the "Message recieved: " prompt
			// and whatever the user inputed immediately after
			// TODO: this should return the output of the command inputted
			int outputPrompt = recv(socketD, strData, sizeof(strData), 0);
			printf("%s", strData);
			int outputBytes = recv(socketD, strData, sizeof(strData), 0);
			printf("%s\n", strData);
		} else {
			break;
		}
	}

	printf("Disconnecting from server\n");
	return 0;
}

int main(int argc, char const *argv[]) {
	// Client configuration
	int socketD = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4740);
	addr.sin_addr.s_addr = INADDR_ANY;

	printf("Connecting to server...\n");
	int connectStatus = connect(socketD, (struct sockaddr*)&addr, sizeof(addr));

	int return_status = 0;
	if (connectStatus == -1) {
		// An invalid status should only return if the server isn't started
		printf("Error! No server found. Please start the server with `make run`.\n");
	} else {
		char strData[255];

		// Receives initial onboarding message
		recv(socketD, strData, sizeof(strData), 0);
		printf("Message: %s\n", strData);
		
		return_status = handle_server(socketD);
		printf("Returned from handle_server with status: %d\n", return_status);
	}

	return return_status;	
}
