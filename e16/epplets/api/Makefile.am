# $Id$

if BUILD_SNPRINTF
SRCS_SNPRINTF = snprintf.c
endif

lib_LTLIBRARIES = libepplet.la

include_HEADERS = epplet.h
libepplet_la_SOURCES = epplet.c $(SRCS_SNPRINTF)
libepplet_la_DEPENDENCIES = $(top_builddir)/config.h epplet.h
libepplet_la_LDFLAGS = -version-info 2:0:0

INCLUDES = -I$(top_srcdir) $(X_CFLAGS) -DEROOT=\"@EROOT@\"

LDADD = -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

EXTRA_DIST = epplet.h.in snprintf.c
