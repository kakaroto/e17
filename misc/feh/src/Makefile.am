# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I. \
-DPREFIX=\""$(prefix)"\"

bin_PROGRAMS      = feh
feh_SOURCES       = main.c main.h feh.h options.c winwidget.c winwidget.h \
filelist.c filelist.h multiwindow.c montage.c imlib.c index.c slideshow.c \
utils.c utils.h modify.c modify.h keyevents.c
feh_LDADD         = -lX11 -lImlib2

test:
	@for i in test[0-9]* ; do \
	  if test -x $$i ; then \
	    echo "Executing test script $$i..." ; \
	    ./$$i ; \
	  fi ; \
	done
	
SUBDIRS = ttfonts
