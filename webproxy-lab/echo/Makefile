CC = gcc
CFLAGS = -g -Wall -I .
LIB = -lpthread

all: echoserveri echoclient

echoserveri: echoserveri.c echo.c csapp.o
	$(CC) $(CFLAGS) -o echoserveri echoserveri.c echo.c csapp.o $(LIB)

echoclient: echoclient.c csapp.o
	$(CC) $(CFLAGS) -o echoclient echoclient.c csapp.o $(LIB)

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f *.o echoserveri echoclient *~
