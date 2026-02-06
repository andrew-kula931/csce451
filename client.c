#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char const *argv[]) {
	int socketD = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(4740);
	addr.sin_addr.s_addr = INADDR_ANY;

	int connectStatus = connect(socketD, (struct sockaddr*)&addr, sizeof(addr));

	if (connectStatus == -1) {
		printf("Error...\n");
	} else {
		char strData[255];

		recv(socketD, strData, sizeof(strData), 0);

		printf("Message: %s\n", strData);
	}

	return 0;	
}
