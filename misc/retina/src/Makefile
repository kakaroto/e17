GTK_INCLUDES = $(shell gtk-config --cflags) 
IMLIB_INCLUDES = $(shell imlib2-config --cflags) 
EVAS_INCLUDES = $(shell evas-config --cflags)

GTK_LIBS = $(shell gtk-config --libs)
IMLIB_LIBS = $(shell imlib2-config --libs)
EVAS_LIBS = $(shell evas-config --libs)

FLAGS = -g -O2 -Wall -I.. $(GTK_INCLUDES) $(IMLIB_INCLUDES) $(EVAS_INCLUDES)
LIBS = -g $(GTK_LIBS) $(IMLIB_LIBS) $(EVAS_LIBS) -ledb
CC = gcc $(FLAGS)

SOURCES = r_gtk.c r_evas.c r_evas_cb.c r_browser.c  r_db.c r_error.c \
	  r_file_load.c retina.c

OBJS = r_gtk.o r_evas.o r_evas_cb.o r_browser.o r_db.o r_error.o \
       r_file_load.o retina.o
       

all: $(OBJS)
	$(CC) $(OBJS) -o retina $(LIBS)

clean:
	rm -fr *~ *.o core retina

r_gtk.o: r_gtk.c retina.h
r_evas.o: r_evas.c retina.h
r_evas_cb.o: r_evas_cb.c retina.h
r_browser.o: r_browser.c retina.h
r_db.o: r_db.c retina.h
r_error.o: r_error.c retina.h
r_file_load.o: r_file_load.c retina.h
retina.o: retina.c retina.h
