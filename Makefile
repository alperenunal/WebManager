CC = gcc
Wflags = -Wall -Wextra -Wpedantic
defines = 
cflags = -I include/
ldflags = 
libs = -pthread

all: build

build:
	gcc -O3 -Wall -Wextra -pedantic -I include/ src/handles.c src/rules.c src/server.c src/main.c -lpthread -o webmanager

debug:
	gcc -g -Wall -Wextra -pedantic -I include/ src/rules.c src/server.c src/main.c -lpthread -o webmanager

run:
	./webmanager

clean:
	rm webmanager
