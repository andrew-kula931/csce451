# CSCE 451 - Remote Shell using Socket Programming

## Makefile Overview

`make compile` - compiles the program and outputs the executables `server` and `client` respctively.

`make run` - Runs `make compile` and then starts up the server in the background. A client instance is then started in the foreground to interact with.

`make stop` - Kills the server process and checks for any orphaned client processes.

## Usage Instructions

1. Run `make run` to start the server and boot into a client interface.
2. Interact with the remote shell
3. (Optional) Open a new terminal window and run `./client` from the root directory to open a second client instance. This will only work if the server is actively running.
4. Close client programs by running `q` or `quit`.
5. Close the server by running `make stop`.
