EVAS_INCLUDES = $(shell evas-config --cflags)
IMLIB2_INCLUDES = $(shell imlib2-config --cflags)
ECORE_INCLUDES = $(shell ecore-config --cflags)

EVAS_LIBS = $(shell evas-config --libs)
IMLIB2_LIBS = $(shell imlib2-config --libs)
ECORE_LIBS = $(shell ecore-config --libs)

FLAGS = -g -O2 -Wall -I.. $(EVAS_INCLUDES) $(IMLIB2_INCLUDES) $(ECORE_INCLUDES)
LIBS = -g $(EVAS_LIBS) $(IMLIB2_LIBS) $(ECORE_LIBS)
CC = gcc $(FLAGS)

SOURCES = r_gui.c r_img.c r_thumb.c r_cb.c retina.c

OBJS = r_gui.o r_img.o r_thumb.o r_cb.o retina.o

all: $(OBJS)
	$(CC) $(OBJS) -o retina $(LIBS)

install:
	mkdir -p /usr/local/share/retina
	mkdir -p /usr/local/share/retina/img
	mkdir -p /usr/local/share/retina/fnt
	cp ./retina /usr/local/bin
	cp -rf ./img/*.png /usr/local/share/retina/img

clean:
	rm -rf *~ *.o core retina

r_gui.o: r_gui.c retina.h
r_img.o: r_img.c retina.h
r_cb.o: r_cb.c retina.h
retina.o: retina.c retina.h
