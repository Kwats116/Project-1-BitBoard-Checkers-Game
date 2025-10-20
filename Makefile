CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

OBJS = main.o checkers.o bitops.o

all: checkers

checkers: $(OBJS)
	$(CC) $(CFLAGS) -o checkers $(OBJS)

main.o: main.c checkers.h
	$(CC) $(CFLAGS) -c main.c

checkers.o: checkers.c checkers.h
	$(CC) $(CFLAGS) -c checkers.c

bitops.o: bitops.c bitops.h
	$(CC) $(CFLAGS) -c bitops.c

clean:
	rm -f *.o checkers

.PHONY: all clean