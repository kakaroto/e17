MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)\" \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@

AM_CFLAGS = -W -Wall -Wno-implicit

bin_PROGRAMS = ewl_config

ewl_config_DEPENDENCIES = $(top_builddir)/src/lib/libewl.la
ewl_config_SOURCES = ewl_config.c
ewl_config_LDADD = $(top_builddir)/src/lib/libewl.la

