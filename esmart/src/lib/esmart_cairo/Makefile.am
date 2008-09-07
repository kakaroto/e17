MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_cairo \
@CAIRO_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_cairo.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Cairo.h

libesmart_cairo_la_SOURCES = \
esmart_cairo.c \
Esmart_Cairo.h

libesmart_cairo_la_LDFLAGS = -version-info @version_info@
libesmart_cairo_la_LIBADD = @CAIRO_LIBS@
