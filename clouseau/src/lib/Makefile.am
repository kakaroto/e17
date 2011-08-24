MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@

EXTRA_DIST =

pkgdir = $(libdir)/clouseau
pkg_LTLIBRARIES = libclouseau.la

libclouseau_la_SOURCES = libclouseau.c

libclouseau_la_LDFLAGS = -module -avoid-version -rdynamic
libclouseau_la_DEPENDENCIES = $(top_builddir)/config.h
libclouseau_la_LIBADD = @EFL_LIBS@
libclouseau_la_CFLAGS = @EFL_CFLAGS@
