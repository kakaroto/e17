MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @E_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)/plugins/views
pkg_LTLIBRARIES = list.la
list_la_SOURCES = list.h list.c
list_ladir = $(pkgdir)
list_la_DATA = list.desktop

list_la_LIBADD = @E_LIBS@
list_la_LDFLAGS = -module -avoid-version
list_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = list_la_DATA

clean-local:
	rm -rf *~
