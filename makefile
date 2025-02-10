CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o node.o

all: simfs

simfs: $(OBJ)
	$(CC) $(CFLAGS) -o simfs $(OBJ)

main.o: main.c node.h
	$(CC) $(CFLAGS) -c main.c

node.o: node.c node.h
	$(CC) $(CFLAGS) -c node.c

clean:
	rm -f *.o simfs
