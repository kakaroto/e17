APPNAME = elogin
IMLIB_INCLUDES = $(shell imlib2-config --cflags)
EVAS_INCLUDES = $(shell evas-config --cflags)
ECORE_INCLUDES = $(shell ecore-config --cflags)
EBITS_INCLUDES = $(shell ebits-config --cflags)
EDB_INCLUDES = $(shell edb-config --cflags)

IMLIB_LIBS = $(shell imlib2-config --libs)
EVAS_LIBS = $(shell evas-config --libs)
ECORE_LIBS = $(shell ecore-config --libs)
EBITS_LIBS = $(shell ebits-config --libs)
EDB_LIBS = $(shell edb-config --libs)
PAM_LIBS = -lpam

FLAGS = -g -O2 -Wall -I.. $(IMLIB_INCLUDES) $(EVAS_INCLUDES) $(ECORE_INCLUDES) $(EBITS_INCLUDES)
LIBS = -g $(IMLIB_LIBS) $(EVAS_LIBS) $(ECORE_LIBS) $(EBITS_LIBS) $(PAM_LIBS) 
CC = gcc $(FLAGS)

SOURCES = auth.c config.c elogin.c entry.c user.c util.c

OBJS = auth.o config.c elogin.o entry.o user.o util.o

all: $(OBJS)
	 $(CC) $(OBJS) -o $(APPNAME) $(LIBS)

clean:
	 rm -fr *~ *.o core $(APPNAME)

auth.o: auth.c auth.h elogin.h
config.o: config.c elogin.h
elogin.o: elogin.c elogin.h auth.h
entry.o: entry.c elogin.h auth.h
user.o: user.c elogin.h auth.h
util.o: util.c elogin.h
