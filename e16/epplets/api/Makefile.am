# $Id$

if BUILD_SNPRINTF
SRCS_SNPRINTF = snprintf.c
endif

lib_LTLIBRARIES = libepplet.la

include_HEADERS = epplet.h
libepplet_la_SOURCES = epplet.c $(SRCS_SNPRINTF)
libepplet_la_CPPFLAGS = -I$(top_srcdir) $(IMLIB2_CFLAGS) $(X_CFLAGS) -D ENLIGHTENMENT_ROOT=\"$(datadir)/e16\"
libepplet_la_LIBADD = $(IMLIB2_LIBS) $(GLX_LIBS) $(X_LIBS) -lXext -lX11
libepplet_la_LDFLAGS = -version-info 2:0:0

EXTRA_DIST = epplet.h.in snprintf.c
