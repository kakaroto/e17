CC=gcc -W -Wall
OBJECTS=\
bg.o \
util.o \
iconbar.o \
container.o \
main.o
LIBRARIES=`evas-config --libs` `ecore-config --libs` `edje-config --libs` `imlib2-config --libs`
INCLUDES=`evas-config --cflags` `ecore-config --cflags` `edje-config --cflags` `imlib2-config --cflags`
PREFIX=/usr/local

###########################################################################
all: iconbar 
clean:
	rm -f *.o iconbar

iconbar: $(OBJECTS)
	$(CC) -g -o $@ $(OBJECTS) $(LIBRARIES)

.o:
	$(CC)  $(INCLUDES) -c  $<

install:
	install iconbar $(PREFIX)/bin
