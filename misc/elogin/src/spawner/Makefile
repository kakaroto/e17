APPNAME = spawner

FLAGS = -g -O2 -Wall -I/usr/include -I/usr/X11R6/include
LIBS = -g -L/usr/X11R6/lib -lX11
CC = gcc $(FLAGS)

SOURCES = spawner.c

OBJS = spawner.o

all: $(OBJS)
	 $(CC) $(OBJS) -o $(APPNAME) $(LIBS)

clean:
	 rm -fr *~ *.o core $(APPNAME)

spawner.o: spawner.c spawner.h
