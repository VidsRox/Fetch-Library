CC = gcc
CFLAGS = -Wall -g
LDFLAGS = 

OBJS = grab.o network.o utils.o http.o fetch.o timer.o

.PHONY: all clean

all: clean grab

grab: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

grab.o: grab.c grab.h fetch.h timer.h
	$(CC) $(CFLAGS) -c grab.c

network.o: network.c network.h
	$(CC) $(CFLAGS) -c network.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

http.o: http.c http.h grab.h utils.h
	$(CC) $(CFLAGS) -c http.c

fetch.o: fetch.c fetch.h http.h network.h utils.h
	$(CC) $(CFLAGS) -c fetch.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c

clean:
	rm -f *.o grab