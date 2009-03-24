
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_xpixmap \
@XPIXMAP_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_xpixmap.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_XPixmap.h

libesmart_xpixmap_la_SOURCES = \
esmart_xpixmap.c \
Esmart_XPixmap.h

libesmart_xpixmap_la_LDFLAGS = -version-info @version_info@
libesmart_xpixmap_la_LIBADD = @XPIXMAP_LIBS@ -L/usr/X11R6/lib
