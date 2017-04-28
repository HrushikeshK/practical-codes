all : server client

server : udp_server.cpp
	g++ -o server udp_server.cpp

client : udp_client.cpp
	g++ -o client udp_client.cpp

clean :
	rm server client