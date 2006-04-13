# $Id$

lib_LTLIBRARIES = libepplet.la

include_HEADERS = epplet.h
libepplet_la_SOURCES = epplet.c snprintf.c
libepplet_la_DEPENDENCIES = $(top_builddir)/config.h epplet.h
libepplet_la_LDFLAGS = -version-info 1:0:0

INCLUDES = -I$(top_srcdir) $(X_CFLAGS) -DEROOT=\"@EROOT@\"

LDADD = -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

EXTRA_DIST = epplet.h.in
