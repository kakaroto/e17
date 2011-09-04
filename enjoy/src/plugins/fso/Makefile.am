MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLIBRARY_DIR=\"$(libdir)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@EDBUS_CFLAGS@

pkgdir = $(libdir)/enjoy
pkg_LTLIBRARIES = fso.la
fso_la_SOURCES = fso.c
fso_la_LIBADD = @EDBUS_LIBS@
fso_la_LDFLAGS = -no-undefined -module -avoid-version
fso_la_LIBTOOLFLAGS = --tag=disable-static
