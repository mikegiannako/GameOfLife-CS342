CC = gcc
CFLAGS = -Wall -g -fopenmp -O2
CFLAGS_SERIAL = -Wall -g -O2 -D SERIAL

all: main

main: main.o matrix.o hashset.o
	$(CC) $(CFLAGS) $^ -o game_of_life.exe

serial_main: serial_main.o serial_matrix.o serial_hashset.o
	$(CC) $(CFLAGS_SERIAL) $^ -o game_of_life.exe

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

serial_%.o:%.c
	$(CC) $(CFLAGS_SERIAL) -o $@ -c $<

clean:
	-rm -f *.exe *.o
