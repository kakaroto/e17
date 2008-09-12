
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/bin \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/bin \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@eyelight_cflags@ \
-Wall -g

bin_PROGRAMS = eyelight

eyelight_SOURCES = eyelight.c
eyelight_LDADD = $(top_builddir)/src/lib/libeyelight.la @eyelight_libs@
eyelight_DEPENDENCIES = $(top_builddir)/src/lib/libeyelight.la
