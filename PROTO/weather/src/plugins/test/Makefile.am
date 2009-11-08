MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/plugins/test -I$(top_srcdir)/src/lib \
	 @E_CFLAGS@

pkgdir = $(pluginsdir)
pkg_LTLIBRARIES = test.la
test_la_SOURCES = test.c
test_la_DEPENDENCIES = $(top_builddir)/config.h
test_la_LIBADD = @E_LIBS@
test_la_LDFLAGS = -module -avoid-version


clean-local:
	rm -rf *~

