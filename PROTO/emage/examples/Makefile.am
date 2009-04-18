MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EMAGE_CFLAGS@

bin_PROGRAMS = emage_example

emage_example_DEPENDENCIES = $(top_builddir)/src/lib/libemage.la
emage_example_LDADD = \
$(top_builddir)/src/lib/libemage.la \
@EMAGE_LIBS@

emage_example_SOURCES = \
emage_example.c

