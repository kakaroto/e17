# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I/usr/X11R6/include \
                    $(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I. \
                    -DPREFIX=\""$(prefix)"\"
LIBOBJS = @LIBOBJS@

bin_PROGRAMS      = bdox
bdox_SOURCES      = main.c dox.h hotspot.c
bdox_LDADD        = -lX11

EXTRA_DIST = 
