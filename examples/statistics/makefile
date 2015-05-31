all: ride_the_bus

ride_the_bus: ride_the_bus.o
	g++ ride_the_bus.o -std=c++1y -O2 -o ride_the_bus

ride_the_bus.o: ride_the_bus.cpp
	g++ -c ride_the_bus.cpp -std=c++1y -O2

clean:
	rm *.o ride_the_bus

