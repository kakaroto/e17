CC=gcc -W -Wall
OBJECTS=\
elicit.o 
LIBRARIES=`evas-config --libs` `ecore-config --libs` `edje-config --libs` `imlib2-config --libs`
INCLUDES=`evas-config --cflags` `ecore-config --cflags` `edje-config --cflags` `imlib2-config --cflags`
PREFIX=/usr/local

###########################################################################
all: elicit 
clean:
	rm -f *.o elicit 

elicit: $(OBJECTS)
	$(CC) -g -o $@ $(OBJECTS) $(LIBRARIES) 

.c.o:
	$(CC)  $(INCLUDES) -c $< 

install:
	install elicit $(PREFIX)/bin
