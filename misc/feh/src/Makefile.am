# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I.

bin_PROGRAMS      = feh
feh_SOURCES       = main.c main.h feh.h options.c winwidget.c winwidget.h \
filelist.c filelist.h multiwindow.c montage.c imlib.c
feh_LDADD         = -lX11 -lImlib2
