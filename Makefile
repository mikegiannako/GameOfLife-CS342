CC = gcc
CFLAGS = -Wall -g -pthread -fopenmp

all: main

main: main.o matrix.o
	$(CC) $(CFLAGS) $^ -o game_of_life.exe

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.exe *.o
