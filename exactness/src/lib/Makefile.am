MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@

EXTRA_DIST = \
	     tsuite_file_data.h

pkgdir = $(libdir)/exactness
pkg_LTLIBRARIES = libexactness.la

libexactness_la_SOURCES = \
			  tsuite_evas_hook.c  \
			  tsuite_file_data.c

libexactness_la_LDFLAGS = -module -avoid-version -rdynamic
libexactness_la_DEPENDENCIES = $(top_builddir)/config.h
libexactness_la_LIBADD = @EFL_LIBS@
libexactness_la_CFLAGS = @EFL_CFLAGS@
