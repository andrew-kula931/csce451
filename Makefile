SERVER_CMD = ./server
CLIENT_CMD = ./client
PID_FILE = server.pid

.PHONY: run stop

compile: client.c server.c
	gcc -o server server.c
	gcc -o client client.c

# Starts server and writes server PID to a file
run:
	@echo "Starting server..."
	@$(SERVER_CMD) & echo $$! > $(PID_FILE)
	@sleep 1
	@echo "Running client..."
	@$(CLIENT_CMD)

# Reads PID file to kill server and checks for any client processes
stop:
	@echo "Stopping server..."
	@if [ -f $(PID_FILE) ]; then \
		kill $$(cat $(PID_FILE)) 2>/dev/null || true; \
		rm -f $(PID_FILE); \
		echo "Server stopped"; \
	else \
		echo "No PID file found"; \
	fi
	@pkill -f $(CLIENT_CMD) || true
	@echo "Client stopped if running"
