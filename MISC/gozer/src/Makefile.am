# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

SUBDIRS = fonts

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -g -O3 -Wall -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I. \
-DPREFIX=\""$(prefix)"\" @GIBLIB_CFLAGS@
LIBOBJS = @LIBOBJS@

bin_PROGRAMS      = gozer
gozer_SOURCES       = main.c gozer.c getopt.c getopt1.c getopt.h gozer.h \
options.c debug.h utils.c utils.h imlib.c
gozer_LDADD         = @GIBLIB_LIBS@
