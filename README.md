# CSCE 451 - Remote Shell using Socket Programming

## Makefile Overview

`make compile` - compiles the program and outputs the executables `server` and `client` respctively.

`make run` - Runs `make compile` and then starts up the server in the background. A client instance is then started in the foreground to interact with.

`make stop` - Kills the server process and checks for any orphaned client processes.

## Usage Instructions# CSCE 451 – Remote Shell Using Socket Programming

## Overview

This project implements a remote shell system using TCP socket programming in C. The system consists of a server and a client.

- The server runs on a machine and listens for incoming TCP connections.
- The client connects to the server, sends shell commands, and displays the output returned by the server.
- The server supports multiple simultaneous clients using fork().
- Each command is executed using execvp(), and both stdout and stderr are sent back to the client.
- The server assumes commands do not require stdin.

---

## Compilation

To compile the project:

    make

or

    make compile

This produces two executables:

    server
    client

---

## Running the Program

### Local Testing (Server and Client on Same Machine)

Start the server:

    ./server

In another terminal window, start the client and specify the host:

    ./client 127.0.0.1

You should see a successful connection message.

---

### Using make run

    make run

This will:
1. Compile the project
2. Start the server in the background
3. Launch a client connected to 127.0.0.1

---

### Connecting to a Remote Machine

If the server is running on another machine:

    ./client <server_ip_address>

Example:

    ./client 192.168.1.25

The client requires the server host as a command line argument.

---

## Using the Remote Shell

After connecting, you will see a server prompt similar to:

    server/root>

You may enter standard UNIX commands such as:

    ls
    ls -l
    pwd
    whoami
    date
    ps -ael
    cat filename.txt

The command will:
1. Be sent to the server
2. Be executed on the server machine
3. Return stdout and stderr output
4. Be displayed on the client terminal

Notes:
- Commands that require interactive input (such as vim, nano, less) are not supported.
- Each command runs independently. Directory changes using cd do not persist between commands.

---

## Multiple Clients

The server supports multiple clients simultaneously.

To open another client:

    ./client 127.0.0.1

Each connection is handled in a separate child process created using fork().

---

## Exiting

To close a client session:

    quit

or

    q

To stop the server:

    make stop

---

## Implementation Details

- Uses TCP sockets (SOCK_STREAM)
- Server binds to port 4740
- Server forks a child process per client connection
- Commands are parsed into argument arrays
- Commands are executed using execvp()
- Output is captured using pipes and sent back to the client
- Parent server process continues listening for new connections

---

## Compilation Environment

This program is intended to compile and run on:

    nuros.unl.edu
1. Run `make run` to start the server and boot into a client interface.
2. Interact with the remote shell
3. (Optional) Open a new terminal window and run `./client` from the root directory to open a second client instance. This will only work if the server is actively running.
4. Close client programs by running `q` or `quit`.
5. Close the server by running `make stop`.
