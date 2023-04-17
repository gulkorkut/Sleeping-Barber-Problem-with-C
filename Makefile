CC = gcc
CFLAGS = -pthread

all: barber

phsp:barber.c 
	$(CC) $(CFLAGS) -o barber barber.c

clean:
	rm -f barber

run:
	./barber 5 2 2 3 4