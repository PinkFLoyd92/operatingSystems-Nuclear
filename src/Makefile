CC=gcc
PROGRAMS=nuclear $(UI_DIR)/ui.o $(REACTOR_DIR)/reactor.o $(SOCKET_DIR)/socket.o
REACTOR_DIR="reactor"
UI_DIR="ui"
SOCKET_DIR="socket"

all: ui.o socket.o reactor.o 
	$(CC) `pkg-config --cflags gtk+-3.0` main.c $(UI_DIR)/ui.o $(SOCKET_DIR)/socket.o $(REACTOR_DIR)/reactor.o -g -o nuclear `pkg-config --libs gtk+-3.0` -lpthread -lcurl

ui.o:
	$(CC) -c `pkg-config --cflags gtk+-3.0` $(UI_DIR)/main_window.c -o $(UI_DIR)/ui.o `pkg-config --libs gtk+-3.0`

reactor.o:
	$(CC) $(SOCKET_DIR)/socket.o -c $(REACTOR_DIR)/reactor.c -o $(REACTOR_DIR)/reactor.o -lpthread

socket.o:
	$(CC) -c $(SOCKET_DIR)/socket.c -o $(SOCKET_DIR)/socket.o -lcurl

clean:
	@rm -f $(PROGRAMS) *.o
