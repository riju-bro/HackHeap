CC = gcc
CFLAGS = -Wall
build: main.c HEAP.c
	$(CC) $(CFLAGS) main.c HEAP.c
