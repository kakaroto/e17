APPNAME = elogin
IMLIB_INCLUDES = $(shell imlib2-config --cflags)
EVAS_INCLUDES = $(shell evas-config --cflags)
EBITS_INCLUDES = $(shell ebits-config --cflags)
ECORE_INCLUDES = $(shell ecore-config --cflags)
EDB_INCLUDES = $(shell edb-config --cflags)

IMLIB_LIBS = $(shell imlib2-config --libs)
EVAS_LIBS = $(shell evas-config --libs)
EBITS_LIBS = $(shell ebits-config --libs)
ECORE_LIBS = $(shell ecore-config --libs)
EDB_LIBS = $(shell edb-config --libs)

FLAGS = -g -O2 -Wall -I.. $(IMLIB_INCLUDES) $(EVAS_INCLUDES) $(EBITS_INCLUDES) $(ECORE_INCLUDES) $(EDB_INCLUDES)
LIBS = -g $(IMLIB_LIBS) $(EVAS_LIBS) $(EBITS_LIBS) $(ECORE_LIBS) $(EDB_LIBS)
CC = gcc $(FLAGS)

SOURCES = auth.c elogin.c

OBJS = auth.o elogin.o 

all: $(OBJS)
	 $(CC) $(OBJS) -o $(APPNAME) $(LIBS)

clean:
	 rm -fr *~ *.o core $(APPNAME)

auth.o: auth.c auth.h elogin.h
elogin.o: elogin.c elogin.h auth.h
