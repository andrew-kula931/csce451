#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int handle_client(int client_id) {
    char serMsg[255] = "Connection successful\n";
    send(client_id, serMsg, sizeof(serMsg), 0);

    while (1 == 1) {
        char prompt[255] = "Server/root> ";
        send(client_id, prompt, sizeof(prompt), 0);

        char clientMsg[255];
        int bytesReceived = recv(client_id, clientMsg, sizeof(clientMsg), 0);
        if (bytesReceived > 0) {
            send(client_id, "Message received: ", sizeof("Message received: "), 0);
            send(client_id, clientMsg, sizeof(clientMsg), 0);

            if (strcmp(clientMsg, "quit\n") == 0 || strcmp(clientMsg, "q\n") == 0) {
                break;
            }
        } 
    }

    return 0;
}

int main(int argc, char const* argv[])
{
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(4740);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr));

    while (1 == 1) {
        listen(servSockD, 10);

        int clientSocket = accept(servSockD, NULL, NULL);

        // Forks the client process and returns after the client disconnects
        int pid = fork();
        if (pid == 0) {
            int client_status = handle_client(clientSocket);
            return client_status;
        }
    }

    return 0;
}

