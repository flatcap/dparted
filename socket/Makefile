simple_server_objects = server_socket.o socket.o simple_server_main.o
simple_client_objects = client_socket.o socket.o simple_client_main.o

CFLAGS	= -Wall -Wextra -Wpedantic

all : simple_server simple_client

simple_server: $(simple_server_objects)
	g++ $(CFLAGS) -o simple_server $(simple_server_objects)

simple_client: $(simple_client_objects)
	g++ $(CFLAGS) -o simple_client $(simple_client_objects)

clean:
	rm -f *.o simple_server simple_client
