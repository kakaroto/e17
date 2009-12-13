MAINTAINERCLEANFILES = Makefile.in

icondir = $(pkgdatadir)
icon_DATA =  google_logo.jpg


AM_CPPFLAGS = -I$(top_srcdir)/src/plugins/google -I$(top_srcdir)/src/lib \
	 @E_CFLAGS@ -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

pkgdir = $(pluginsdir)
pkg_LTLIBRARIES = google.la
google_la_SOURCES = google.c
google_la_DEPENDENCIES = $(top_builddir)/config.h
google_la_LIBADD = @E_LIBS@
google_la_LDFLAGS = -module -avoid-version


clean-local:
	rm -rf *~

