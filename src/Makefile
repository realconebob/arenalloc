CC = gcc
SHELL = /usr/bin/bash

CFLAGS = -std=c2x -Wall -Wextra -Wpedantic -pedantic-errors -fanalyzer -Wanalyzer-too-complex -ggdb -g3 -O0

.PHONY: all c clean val

all: main

c clean:
	rm -rvf main $(wildcard *.o)

val:
	$(MAKE) all
	valgrind --leak-check=yes ./main

main: main.o arena.o arena.h
main.o: main.c arena.h
arena.o: arena.c arena.h