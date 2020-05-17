#OpenMP Flags Etc.
FLAGS = -g -std=c++14 -I restbed/distribution/include/ -L restbed/distribution/library/
LIBS = -lrestbed
CC = g++


restServer: webServer.cc
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o restServer
