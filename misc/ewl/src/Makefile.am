# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

SUBDIRS = themes

LDFLAGS           = -L/usr/X11R6/lib
INCLUDES          = -I/usr/X11R6/include $(X_CFLAGS) \
-I$(prefix)/include -I$(includedir) -I. \
-DPREFIX=\""$(prefix)"\" $(X_CFLAGS)

lib_LTLIBRARIES = libewl.la
bin_PROGRAMS = ewltest ewldbtool

installed_headers_DATA = \
ewl.h ewlcore.h ewlcontainer.h ewlbox.h ewlwindow.h ewlbutton.h

installed_headersdir = $(prefix)/include/ewl

libewl_la_SOURCES = \
debug.c debug.h \
event.c event.h \
event_types.h \
ewl.h \
ewldb.c ewldb.h \
ewlbox.c ewlbox.h \
ewlbutton.c ewlbutton.h \
ewlcontainer.c ewlcontainer.h \
ewlcore.h \
ewldbtool.c \
ewllabel.c ewllabel.h \
ewlwindow.c ewlwindow.h \
image.c image.h \
imlib.c imlib.h \
includes.h \
layout.c layout.h \
ll.c ll.h \
mwm.h \
option.c option.h \
state.c state.h \
theme.c theme.h \
translators.c translators.h \
types.h \
util.c util.h \
widget.c widget.h
libewl_la_LIBADD         = -lX11 -lImlib2 -ldb

ewltest_DEPENDENCIES = libewl.la
ewltest_LDADD = libewl.la -L$(libdir) -L$(prefix)/lib $(X_LIBS) $(LIBS)
ewltest_LDFLAGS = -rpath $(libdir):$(pkglibdir)
ewltest_SOURCES = main.c

ewldbtool_DEPENDENCIES = libewl.la
ewldbtool_LDADD = libewl.la -L$(libdir) -L$(prefix)/lib $(X_LIBS) $(LIBS)
ewldbtool_LDFLAGS = -rpath $(libdir):$(pkglibdir)
ewldbtool_SOURCES = ewldbtool.c

