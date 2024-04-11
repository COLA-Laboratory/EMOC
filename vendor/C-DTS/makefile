CC=g++
CXXFLAGS=-g -std=c++11 -Wall

all: Arm.o DTS.o main.o
	$(CC) $(CXXFLAGS) Arm.o DTS.o main.o -o DTS


clean:
	rm -f DTS
	rm -f *.o
