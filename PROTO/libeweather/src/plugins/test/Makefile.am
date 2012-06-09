MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/plugins/test \
-I$(top_srcdir)/src/lib \
@EFL_EWEATHER_BUILD@ \
@EWEATHER_CFLAGS@

pkgdir = $(pkglibdir)/plugins
pkg_LTLIBRARIES = test.la

test_la_SOURCES = test.c
test_la_LIBADD = $(top_builddir)/src/lib/libeweather.la @EWEATHER_LIBS@
test_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
test_la_LIBTOOLFLAGS = --tag=disable-static

clean-local:
	rm -rf *~
