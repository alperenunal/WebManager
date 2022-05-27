CC = gcc
Wflags = -Wall -Wextra -Wpedantic
defines = -D_GNU_SOURCE
cflags = -I include/
ldflags = 
libs = -pthread
files = src/main.c src/server.c src/rules.c src/handles.c src/cpu.c src/process.c

all: build run clean

build:
	$(CC) $(defines) $(Wflags) $(cflags) $(ldflags) $(files) $(libs) -o webmanager

debug:
	$(CC) $(defines) -g $(Wflags) $(cflags) $(ldflags) $(files) $(libs)

run:
	./webmanager

clean:
	rm webmanager
