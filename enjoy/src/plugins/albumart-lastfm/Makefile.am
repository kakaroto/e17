MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLIBRARY_DIR=\"$(libdir)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@EFL_CFLAGS@

pkgdir = $(libdir)/enjoy
pkg_LTLIBRARIES = albumart-lastfm.la
albumart_lastfm_la_SOURCES = albumart-lastfm.c
albumart_lastfm_la_LDFLAGS = -no-undefined -module -avoid-version
albumart_lastfm_la_LIBTOOLFLAGS = --tag=disable-static
