CC = gcc
CFLAGS = -O3
OBJS = board.o input.o main.o protocol.o search.o util.o

all:	$(OBJS)
		$(CC) -o matto -g -pthread $(OBJS)

%.o:	%.c util.h
		$(CC) -c -g $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~ matto
