AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EXTRACXXFLAGS@ \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEV8_CFLAGS@

bin_PROGRAMS = elev8

elev8_SOURCES = \
main.cc main.h

elev8_LDADD = @ELEV8_LIBS@ @V8_LIBS@
