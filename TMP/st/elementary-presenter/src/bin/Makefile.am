AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = elementary_presenter

elementary_presenter_SOURCES = main.c
elementary_presenter_LDADD = @ELEMENTARY_LIBS@
