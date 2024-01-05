CC = mpic++ 
SRCS = ./src/*.cpp
INC = ./src/
OPTS = -std=c++17 -Wall -lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

EXEC = /bin/nbody

all: clean compile

compile:
	$(CC) $(SRCS) $(OPTS) -I$(INC) -o $(EXEC) -g

clean:
	rm -f $(EXEC)
