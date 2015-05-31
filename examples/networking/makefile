all: network_test test_client

network_test: network_test.o server.o socket.o
	g++ network_test.o server.o socket.o -pthread -std=c++1y -g -O2 -o network_test

test_client: test_client.o socket.o
	g++ test_client.o socket.o -std=c++1y -g -O2 -o test_client

network_test.o: network_test.cpp
	g++ -c network_test.cpp -std=c++1y -g -O2

server.o: ../../networking//server.cpp
	g++ -c ../../networking//server.cpp -std=c++1y -g -O2
    
socket.o: ../../networking//osl/socket.cpp
	g++ -c ../../networking/osl/socket.cpp -std=c++1y -g -O2

clean:
	rm *.o network_test test_client

