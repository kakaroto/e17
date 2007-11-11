
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir)/src/lib/esmart_thumb \
@THUMB_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_thumb.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Thumb.h

libesmart_thumb_la_SOURCES = \
esmart_thumb.c \
Esmart_Thumb.h

libesmart_thumb_la_LDFLAGS = -version-info @version_info@
libesmart_thumb_la_LIBADD = @THUMB_LIBS@
