MAINTAINERCLEANFILES = Makefile.in

icondir = $(pkgdatadir)
icon_DATA = yahoo_logo.jpg

AM_CPPFLAGS = \
-I$(top_srcdir)/src/plugins/yahoo \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_EWEATHER_BUILD@ \
@EWEATHER_CFLAGS@

pkgdir = $(pkglibdir)/plugins
pkg_LTLIBRARIES = yahoo.la

yahoo_la_SOURCES = yahoo.c
yahoo_la_LIBADD = $(top_builddir)/src/lib/libeweather.la @EWEATHER_LIBS@
yahoo_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
yahoo_la_LIBTOOLFLAGS = --tag=disable-static

EXTRA_DIST = yahoo_logo.jpg

clean-local:
	rm -rf *~
