APP=    ewltest
DBTOOL= ewldbtool
CC=     cc
RM=     rm -f 
CFLAGS= -O2 -g -pg -W -Wall -ansi -pedantic -I. -I.. `evas-config --cflags`
LIBS=   `evas-config --libs` -ldb
OBJS=   api.o box.o container.o error.o ewldb.o font.o event.o hash.o layout.o list.o main.o object.o timer.o theme.o translators.o util.o widget.o window.o x.o
DBOBJS= ewldb.o list.o util.o

all: $(APP)

$(APP): $(OBJS) $(DBTOOL) 
	$(CC) -o $(APP) $(LIBS) $(OBJS)

$(DBTOOL): dbtool.c $(DBOBJS)
	$(CC) -o $(DBTOOL) $(CFLAGS) $(LIBS) $(DBOBJS) dbtool.c

%.o:%.c
	$(CC) $(CFLAGS) -c $<
	
clean: 
	$(RM) $(DBTOOL) $(APP) $(APP).out $(OBJS) core
