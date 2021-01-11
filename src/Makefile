CC = g++
LIBS = -std=c++11 -O3
OBJS = main.o lib.o
fpga_project: $(OBJS)
	$(CC) $(OBJS) -o ../bin/fpga_project
main.o: main.cpp lib.h
	$(CC) -c main.cpp $(LIBS)
lib.o: lib.cpp lib.h
	$(CC) -c lib.cpp $(LIBS)
clean:
	rm *.o
