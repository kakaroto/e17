# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I. -I$(top_srcdir)/dox -I/usr/X11R6/include \
                    -I$(top_srcdir)/src \
                    $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
                    -DPREFIX=\""$(prefix)"\"

LIBOBJS = @LIBOBJS@

bin_PROGRAMS      = bdox
bdox_SOURCES      = main.c dox.h hotspot.c style.c utils.c style.h
bdox_LDADD        = -lX11

EXTRA_DIST = 
