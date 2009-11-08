MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/plugins/yahoo -I$(top_srcdir)/src/lib \
	 @E_CFLAGS@

pkgdir = $(pluginsdir)
pkg_LTLIBRARIES = yahoo.la
yahoo_la_SOURCES = yahoo.c
yahoo_la_DEPENDENCIES = $(top_builddir)/config.h
yahoo_la_LIBADD = @E_LIBS@
yahoo_la_LDFLAGS = -module -avoid-version


clean-local:
	rm -rf *~

