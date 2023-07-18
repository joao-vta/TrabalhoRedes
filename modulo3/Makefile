CC=g++
INCLUDES = ./includes
SOURCES = sources/*.cpp
SERVER_PROG = server-main.cpp
CLIENT_PROG = client-main.cpp
CLIENT_BIN = ./client
SERVER_BIN = ./server
FLAGS = -g -Wall -Werror -pthread
VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full -s

all: clean server client

server:
	@$(CC) -o $(SERVER_BIN) $(SERVER_PROG) $(SOURCES) -I$(INCLUDES) $(FLAGS)

client:
	@$(CC) -o $(CLIENT_BIN) $(CLIENT_PROG) $(SOURCES) -I$(INCLUDES) $(FLAGS)

runServer:
	@./$(SERVER_BIN)

runClient:
	@./$(CLIENT_BIN)

clean:
	rm $(CLIENT_BIN) $(SERVER_BIN) *.bin vgcore.* -f

valgrind:
	valgrind $(VFLAGS) 

valgrind_client:
	valgrind $(VFLAGS)
