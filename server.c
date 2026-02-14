#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVER_PORT 4740
#define MAX_BUFFER_SIZE 256
#define MAX_OUTPUT_SIZE 4096

static void trim_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

static ssize_t execvp_capture_output(const char *command_in, char *out, size_t out_sz) {
    if (out_sz == 0) {
        return 0;
    }

    out[0] = '\0';

    // Work on a mutable copy so we can tokenize
    char cmd_buf[MAX_BUFFER_SIZE];
    strncpy(cmd_buf, command_in, sizeof(cmd_buf) - 1);
    cmd_buf[sizeof(cmd_buf) - 1] = '\0';
    trim_newline(cmd_buf);

    // Tokenize into args[] for execvp
    char *args[64];
    int argc = 0;
    char *saveptr = NULL;
    char *tok = strtok_r(cmd_buf, " \t", &saveptr);
    while (tok != NULL && argc < 63) {
        args[argc++] = tok;
        tok = strtok_r(NULL, " \t", &saveptr);
    }
    args[argc] = NULL;

    if (argc == 0) {
        return 0;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        snprintf(out, out_sz, "Error creating pipe\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: redirect stdout and stderr to the pipe, then exec
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        execvp(args[0], args);
        perror("execvp");
        _exit(127);
    }

    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        snprintf(out, out_sz, "Error forking\n");
        return -1;
    }

    // Parent: read all output
    close(pipefd[1]);

    size_t total = 0;
    while (total < out_sz - 1) {
        ssize_t n = read(pipefd[0], out + total, out_sz - 1 - total);
        if (n <= 0) {
            break;
        }
        total += (size_t)n;
    }
    close(pipefd[0]);
    out[total] = '\0';

    int status = 0;
    waitpid(pid, &status, 0);

    return (ssize_t)total;
}

/**
 * Handles communication with a connected client.
 * @param client_id The socket file descriptor of the connected client.
 * @return 0 on successful completion.
 */
static int handle_client(int client_id) {
    const char *serMsg = "Connection successful\n";
    send(client_id, serMsg, strlen(serMsg), 0);

    while (1 == 1) {
        // To simulate a remote environment the default location is added
        const char *prompt = "server/root> ";
        send(client_id, prompt, strlen(prompt), 0);

        char clientMsg[MAX_BUFFER_SIZE];
        int bytesReceived = recv(client_id, clientMsg, sizeof(clientMsg) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }
        clientMsg[bytesReceived] = '\0';

        if (strcmp(clientMsg, "quit\n") == 0 || strcmp(clientMsg, "q\n") == 0) {
            break;
        }

        char output[MAX_OUTPUT_SIZE];
        execvp_capture_output(clientMsg, output, sizeof(output));

        // Avoid sending 0 bytes (client could block waiting for a response)
        if (output[0] == '\0') {
            strcpy(output, "\n");
        }

        send(client_id, output, strlen(output), 0);
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    // Server configuration
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server to the specified address and port
    if (bind(servSockD, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        close(servSockD);
        return 1;
    }

    // Maxes out at 10 client instances
    if (listen(servSockD, 10) < 0) {
        perror("listen");
        close(servSockD);
        return 1;
    }

    while (1 == 1) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(servSockD, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client (%s:%d) Connected to the server successfully\n", client_ip, ntohs(clientAddr.sin_port));
        fflush(stdout);

        int pid = fork();
        if (pid == 0) {
            // Child handles this client
            close(servSockD);
            int client_status = handle_client(clientSocket);
            close(clientSocket);
            return client_status;
        }

        // Parent keeps accepting new clients
        close(clientSocket);

        // Reap any finished children without blocking
        while (waitpid(-1, NULL, WNOHANG) > 0) {
        }
    }

    return 0;
}
