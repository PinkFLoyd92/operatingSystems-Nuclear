CC=gcc
PROGRAMS=nuclear $(UI_DIR)/ui.o $(REACTOR_DIR)/reactor.o $(SOCKET_DIR)/socket.o
REACTOR_DIR="reactor"
UI_DIR="ui"
SOCKET_DIR="socket"

all: socket.o reactor.o 
	$(CC) main.c $(UI_DIR)/ui.o $(SOCKET_DIR)/socket.o $(REACTOR_DIR)/reactor.o -g -o nuclear -lpthread -lcurl -lrt -D_GNU_SOURCE -std=gnu99

reactor.o:
	$(CC) $(SOCKET_DIR)/socket.o -c $(REACTOR_DIR)/reactor.c -o $(REACTOR_DIR)/reactor.o -lpthread -lrt -std=gnu99

socket.o:
	$(CC) -c $(SOCKET_DIR)/socket.c -o $(SOCKET_DIR)/socket.o -lcurl -std=gnu99

clean:
	@rm -f $(PROGRAMS) *.o -std=gnu99
