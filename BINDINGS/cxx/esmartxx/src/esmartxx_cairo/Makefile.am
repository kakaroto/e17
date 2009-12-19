
lib_LTLIBRARIES = \
	libesmartxx_cairo.la

libesmartxx_cairo_la_SOURCES = \
	Cairo.cpp

libesmartxx_cairo_la_LIBADD =                \
	$(EFL_LIBS)                \
	$(ESMART_CAIRO_LIBS)

EXTRA_DIST = \
	$(esmartxx_cairo_HEADERS)\
	$(libesmartxx_cairo_HEADERS)

libesmartxx_cairo_HEADERS = \
	Cairo.h

libesmartxx_cairodir = \
	$(pkgincludedir)

AM_CPPFLAGS = \
	$(EFL_CFLAGS) \
	$(ESMART_CAIRO_CFLAGS)

## File created by the gnome-build tools

