APPNAME = elogin
IMLIB_INCLUDES = $(shell imlib2-config --cflags)
EVAS_INCLUDES = $(shell evas-config --cflags)
ECORE_INCLUDES = $(shell ecore-config --cflags)

IMLIB_LIBS = $(shell imlib2-config --libs)
EVAS_LIBS = $(shell evas-config --libs)
ECORE_LIBS = $(shell ecore-config --libs)

FLAGS = -g -O2 -Wall -I.. $(IMLIB_INCLUDES) $(EVAS_INCLUDES) $(ECORE_INCLUDES)
LIBS = -g $(IMLIB_LIBS) $(EVAS_LIBS) $(ECORE_LIBS)
CC = gcc $(FLAGS)

SOURCES = auth.c elogin.c

OBJS = auth.o elogin.o 

all: $(OBJS)
	 $(CC) $(OBJS) -o $(APPNAME) $(LIBS)

clean:
	 rm -fr *~ *.o core $(APPNAME)

auth.o: auth.c auth.h elogin.h
elogin.o: elogin.c elogin.h auth.h
