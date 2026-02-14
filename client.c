#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 4740
#define MAX_LINE 255
#define MAX_OUTPUT 4096

static int resolve_host_ipv4(const char *host, struct in_addr *out_addr) {
    // Try dotted-quad first
    if (inet_pton(AF_INET, host, out_addr) == 1) {
        return 0;
    }

    // Fall back to hostname lookup
    struct hostent *he = gethostbyname(host);
    if (he == NULL || he->h_addrtype != AF_INET || he->h_addr_list[0] == NULL) {
        return -1;
    }

    memcpy(out_addr, he->h_addr_list[0], sizeof(struct in_addr));
    return 0;
}

/**
 * Provides a separate function to handle interactions with the server
 * @param socketD The socket id
 * @param initial_cmd Optional first command (with trailing \n), may be NULL
 * @return 0 on successful completion
 */
static int handle_server(int socketD, const char *initial_cmd) {
    int used_initial = 0;

    while (1 == 1) {
        char prompt[MAX_LINE + 1];
        int bytesReceived = recv(socketD, prompt, MAX_LINE, 0);
        if (bytesReceived <= 0) {
            break;
        }
        prompt[bytesReceived] = '\0';

        // The first part is the server directory prompt (server/root>)
        printf("%s", prompt);
        fflush(stdout);

        char input[MAX_LINE + 1];

        // If a command was given on the command line, send it once, then fall back to stdin.
        if (!used_initial && initial_cmd != NULL && initial_cmd[0] != '\0') {
            strncpy(input, initial_cmd, MAX_LINE);
            input[MAX_LINE] = '\0';
            used_initial = 1;

            // Echo so the output looks like an interactive shell session.
            printf("%s", input);
            fflush(stdout);
        } else {
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }
        }

        // Client will not close/exit until the user enters quit
        if (strcmp(input, "quit\n") == 0 || strcmp(input, "q\n") == 0) {
            send(socketD, input, strlen(input), 0);
            break;
        }

        send(socketD, input, strlen(input), 0);

        // Wait for the server to respond with the command output
        char output[MAX_OUTPUT + 1];
        int outputBytes = recv(socketD, output, MAX_OUTPUT, 0);
        if (outputBytes <= 0) {
            break;
        }
        output[outputBytes] = '\0';
        printf("%s", output);

        // Keep the terminal nice if the server output didn't end with a newline
        if (outputBytes > 0 && output[outputBytes - 1] != '\n') {
            printf("\n");
        }
    }

    printf("Disconnecting from server\n");
    return 0;
}

int main(int argc, char const *argv[]) {
    const char *host = NULL;
    char initial_cmd[MAX_LINE + 1] = {0};
    const char *initial_cmd_ptr = NULL;

    // Assignment requirement: host and command can be specified on the command line.
    // Backward-compatible default so your existing Makefile still works.
    if (argc >= 2) {
        host = argv[1];
    } else {
        host = "127.0.0.1";
    }

    if (argc >= 3) {
        // Build a single command line from argv[2..] and add a trailing newline
        size_t pos = 0;
        for (int i = 2; i < argc; i++) {
            size_t len = strlen(argv[i]);
            if (pos + len + 2 >= sizeof(initial_cmd)) {
                break;
            }
            memcpy(initial_cmd + pos, argv[i], len);
            pos += len;
            if (i < argc - 1 && pos + 1 < sizeof(initial_cmd)) {
                initial_cmd[pos++] = ' ';
            }
        }
        if (pos + 1 < sizeof(initial_cmd)) {
            initial_cmd[pos++] = '\n';
        }
        initial_cmd[pos] = '\0';
        initial_cmd_ptr = initial_cmd;
    }

    // Client configuration
    int socketD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketD < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);

    if (resolve_host_ipv4(host, &addr.sin_addr) != 0) {
        fprintf(stderr, "Error! Could not resolve host: %s\n", host);
        close(socketD);
        return 1;
    }

    int connectStatus = connect(socketD, (struct sockaddr *)&addr, sizeof(addr));

    if (connectStatus == -1) {
        printf("Error! No server found. Please start the server with `make run`.\n");
        close(socketD);
        return 1;
    }

    // Client-side required message
    char server_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, server_ip, sizeof(server_ip));
    printf("Connected to the server (%s:%d) successfully\n", server_ip, SERVER_PORT);

    // Optional server onboarding message (ignore content; keeps protocol compatible)
    char onboarding[MAX_LINE + 1];
    int n = recv(socketD, onboarding, MAX_LINE, 0);
    if (n > 0) {
        onboarding[n] = '\0';
    }

    int return_status = handle_server(socketD, initial_cmd_ptr);

    close(socketD);
    return return_status;
}
