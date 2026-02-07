#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256
#define MAX_OUTPUT_SIZE 4096

/**
 * Handles communication with a connected client.
 * @param client_id The socket file descriptor of the connected client.
 * @return 0 on successful completion.
 */
int handle_client(int client_id) {
    char serMsg[255] = "Connection successful\n";
    send(client_id, serMsg, sizeof(serMsg), 0);

    while (1 == 1) {
        // To simulate a remote enviroment the default location is added
        char prompt[255] = "server/root> ";
        send(client_id, prompt, sizeof(prompt), 0);

        char clientMsg[255];
        int bytesReceived = recv(client_id, clientMsg, sizeof(clientMsg), 0);

        if (bytesReceived > 0) {
            FILE *fp;
            char buffer[MAX_BUFFER_SIZE];
            char output[MAX_OUTPUT_SIZE] = "";

            // Calls the command the client prompted
            fp = popen(clientMsg, "r");
            if (fp == NULL) {
                send(client_id, "Error executing command\n", sizeof("Error executing command\n"), 0);
            } else {
                // The output may be more than one line so loop over the lines until everything is retrieved
                while (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {

                    if (strlen(output) + strlen(buffer) >= MAX_OUTPUT_SIZE) {
                        break;
                    }

                    strcat(output, buffer);
                }

                int command_status = pclose(fp);

                send(client_id, output, strlen(output), 0);
            }

            if (strcmp(clientMsg, "quit\n") == 0 || strcmp(clientMsg, "q\n") == 0) {
                break;
            }
        } 
    }

    return 0;
}

int main(int argc, char const* argv[])
{
    // Server configuration
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(4740);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Binds the server to the specified address and port
    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr));

    while (1 == 1) {
        // Maxes out at 10 client instances
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

