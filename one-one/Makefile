CC = gcc
CFLAGS = -Wall -pthread -g

all: uthread

uthread: main.o uthread.o
	$(CC) $(CFLAGS) -o uthread main.o uthread.o

main.o: main.c uthread.h
	$(CC) $(CFLAGS) -c main.c

uthread.o: uthread.c uthread.h
	$(CC) $(CFLAGS) -c uthread.c

clean:
	rm -f uthread *.o

