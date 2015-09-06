# Specify compiler
COMP = g++ -lssl -lcrypto -std=c++1y -O2

# Specify target
all: crypto_test

# Build executable
crypto_test: crypto_test.o crypto.o
	$(COMP) crypto_test.o crypto.o -g -o crypto_test

# Build test server object
crypto_test.o: crypto_test.cpp
	$(COMP) -c crypto_test.cpp -g

# Build server library object
crypto.o: ../../cryptography/crypto.cpp
	$(COMP) -c ../../cryptography/crypto.cpp -g

# Clean build
clean:
	rm *.o crypto_test
