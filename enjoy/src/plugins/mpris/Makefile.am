MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLIBRARY_DIR=\"$(libdir)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@EDBUS_CFLAGS@ \
@ECORE_CFLAGS@

pkgdir = $(libdir)/enjoy
pkg_LTLIBRARIES = mpris.la
mpris_la_SOURCES = mpris.c
mpris_la_LIBADD = @EDBUS_LIBS@ @ECORE_LIBS@
mpris_la_LDFLAGS = -no-undefined -module -avoid-version
mpris_la_LIBTOOLFLAGS = --tag=disable-static
