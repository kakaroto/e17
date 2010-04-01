MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = converter-sample

converter_sample_LDADD = @ELEMENTARY_LIBS@
converter_sample_SOURCES = list.c pager.c main.c

noinst_HEADERS = gettext.h gui.h
EXTRA_DIST = gettext.h gui.h
