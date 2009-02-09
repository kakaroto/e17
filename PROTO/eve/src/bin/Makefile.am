MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = eve

eve_SOURCES = \
main.c \
eve_scrolled_webview.c \
eve_navigator.c

eve_CPPFLAGS = \
-I$(top_srcdir) \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EVE_CFLAGS@

eve_LDADD = @EVE_LIBS@

eve_LDFLAGS = @ld_enable_auto_import@ @ld_as_needed@

EXTRA_DIST = eve.h
