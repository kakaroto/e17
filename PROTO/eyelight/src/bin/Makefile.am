
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_builddir) \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EYELIGHT_BIN_CFLAGS@

bin_PROGRAMS = eyelight

eyelight_SOURCES = eyelight.c
eyelight_LDADD = $(top_builddir)/src/lib/libeyelight.la @EYELIGHT_BIN_LIBS@
