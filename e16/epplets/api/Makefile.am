# $Id$

lib_LTLIBRARIES = libepplet.la

include_HEADERS =
libepplet_la_SOURCES = epplet.c snprintf.c
libepplet_la_DEPENDENCIES = $(top_builddir)/config.h
libepplet_la_LDFLAGS = -version-info 1:0:1

INCLUDES = -I. -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include $(X_CFLAGS)
LDADD = -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

EXTRA_DIST = epplet.h.in
