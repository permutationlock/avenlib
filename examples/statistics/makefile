# Specify compiler
COMP = clang++ -std=c++1y -O2

# Specify target
all: ride_the_bus

# Build executable
ride_the_bus: ride_the_bus.o
	$(COMP) ride_the_bus.o -o ride_the_bus

# Build simulation object
ride_the_bus.o: ride_the_bus.cpp
	$(COMP) -c ride_the_bus.cpp

# Clean build
clean:
	rm *.o ride_the_bus
