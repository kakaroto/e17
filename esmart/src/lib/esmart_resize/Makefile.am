
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_resize \
@RESIZE_CFLAGS@ @EINA_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_resize.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Resize.h

libesmart_resize_la_SOURCES = \
esmart_resize.c \
Esmart_Resize.h

libesmart_resize_la_LDFLAGS = -version-info @version_info@
libesmart_resize_la_LIBADD = @RESIZE_LIBS@ @EINA_CFLAGS@
