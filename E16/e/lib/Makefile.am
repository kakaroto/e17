libe16dir = $(pkglibdir)

if BUILD_ROOTHACKLIB

libe16_LTLIBRARIES = libhack.la

libhack_la_SOURCES  = e16_hack.c
libhack_la_CPPFLAGS = -I$(top_srcdir)/src $(X_CFLAGS) $(CWARNFLAGS)

libhack_la_LIBADD   = $(DLOPEN_LIBS)
libhack_la_LDFLAGS  = -avoid-version

endif
