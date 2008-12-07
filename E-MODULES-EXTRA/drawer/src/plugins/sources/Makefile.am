MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @E_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)/plugins/sources
pkg_LTLIBRARIES = launcher.la
launcher_la_SOURCES = launcher.h launcher.c
launcher_ladir = $(pkgdir)
launcher_la_DATA = launcher.desktop

launcher_la_LIBADD = @E_LIBS@
launcher_la_LDFLAGS = -module -avoid-version
launcher_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = launcher_la_DATA

clean-local:
	rm -rf *~
