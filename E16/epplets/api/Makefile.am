
if BUILD_SNPRINTF
SRCS_SNPRINTF = snprintf.c
endif

if BUILD_GLX
EPPLET_LIB_GLX = libepplet_glx.la
endif

lib_LTLIBRARIES = libepplet.la $(EPPLET_LIB_GLX)

include_HEADERS = epplet.h

libepplet_la_SOURCES = epplet.c $(SRCS_SNPRINTF)
libepplet_la_CPPFLAGS = -I$(top_srcdir) $(IMLIB2_CFLAGS) $(X_CFLAGS) -D ENLIGHTENMENT_ROOT=\"$(datadir)/e16\"
libepplet_la_LIBADD = $(IMLIB2_LIBS) $(X_LIBS) -lXext -lX11
libepplet_la_LDFLAGS = -version-info 3:0:0

libepplet_glx_la_SOURCES = epplet_glx.c
libepplet_glx_la_CPPFLAGS = -I$(top_srcdir) $(X_CFLAGS)
libepplet_glx_la_LIBADD = libepplet.la $(GLX_LIBS)
libepplet_glx_la_LDFLAGS = -version-info 3:0:0

EXTRA_DIST = epplet.h.in snprintf.c epplet_glx.c
