all: server client

run_server: server
	@./server

run_client: client
	@./client

server:
	@rm -f server
	@gcc server.c -lpthread -o server

client:
	@rm -f client
	@gcc client.c -lpthread -o client
