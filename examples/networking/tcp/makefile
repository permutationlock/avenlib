# Specify compiler
COMP = clang++ -std=c++1y -O2

# Specify target
all: network_test test_client

# Build executable
network_test: network_test.o server.o socket.o
	$(COMP) network_test.o server.o socket.o -pthread -g -o network_test

# Build test client object
test_client: test_client.o socket.o
	$(COMP) test_client.o socket.o -g -o test_client

# Build test server object
network_test.o: network_test.cpp
	$(COMP) -c network_test.cpp -g

# Build server library object
server.o: ../../../networking/server.cpp
	$(COMP) -c ../../../networking/server.cpp -g
    
# Build socket library object
socket.o: ../../../networking/osl/socket.cpp
	$(COMP) -c ../../../networking/osl/socket.cpp -g

# Clean build
clean:
	rm *.o network_test test_client

