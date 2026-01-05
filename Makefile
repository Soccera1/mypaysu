CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c99
CXXFLAGS = -Wall -Wextra -std=c++11 -pthread
LDFLAGS = -lX11 -lXft -lfontconfig -I/usr/include/freetype2

all: backend frontend

backend: src/backend/server.cpp
	$(CXX) $(CXXFLAGS) -o mypaysu-server src/backend/server.cpp

frontend: src/frontend/main.c
	$(CC) $(CFLAGS) -o mypaysu-client src/frontend/main.c $(LDFLAGS) -I/usr/include/freetype2

clean:
	rm -f mypaysu-server mypaysu-client
