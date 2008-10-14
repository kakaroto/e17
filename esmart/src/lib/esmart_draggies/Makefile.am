
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_draggies \
@DRAGGIES_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_draggies.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Draggies.h

libesmart_draggies_la_SOURCES = \
esmart_draggies.c \
Esmart_Draggies.h

libesmart_draggies_la_LDFLAGS = -version-info @version_info@
libesmart_draggies_la_LIBADD = @DRAGGIES_LIBS@
