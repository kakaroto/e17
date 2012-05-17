MAINTAINERCLEANFILES = Makefile.in

icondir = $(pkgdatadir)
icon_DATA = google_logo.jpg

AM_CPPFLAGS = \
-I$(top_srcdir)/src/plugins/google \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EWEATHER_CFLAGS@

pkgdir = $(pkglibdir)/plugins
pkg_LTLIBRARIES = google.la

google_la_SOURCES = google.c
google_la_LIBADD = @EWEATHER_LIBS@
google_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
google_la_LIBTOOLFLAGS = --tag=disable-static

EXTRA_DIST = google_logo.jpg

clean-local:
	rm -rf *~
