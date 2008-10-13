
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_trans_x11 \
@TRANSX11_CFLAGS@ @EINA_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_trans_x11.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Trans_X11.h

libesmart_trans_x11_la_SOURCES = \
esmart_trans_x11.c \
Esmart_Trans_X11.h

libesmart_trans_x11_la_LDFLAGS = -version-info @version_info@
libesmart_trans_x11_la_LIBADD = @EINA_LIBS@ @TRANSX11_LIBS@ -L/usr/X11R6/lib
