# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I. \
-DPREFIX=\""$(prefix)"\"
LIBOBJS = @LIBOBJS@

bin_PROGRAMS      = feh
feh_SOURCES       = main.c getopt.c getopt1.c getopt.h feh.h options.c \
winwidget.c winwidget.h menu.c menu.h structs.h \
filelist.c filelist.h multiwindow.c montage.c imlib.c index.c slideshow.c \
utils.c utils.h keyevents.c timers.c list.c collage.c debug.h
feh_LDADD         = -lX11

images_DATA = about.png
imagesdir=$(prefix)/share/feh/images
EXTRA_DIST = $(images_DATA)

test:
	@for i in test[0-9]* ; do \
	  if test -x $$i ; then \
	    echo "Executing test script $$i..." ; \
	    ./$$i ; \
	  fi ; \
	done

testclean:
	-rm -f _test*.jpg

SUBDIRS = ttfonts
