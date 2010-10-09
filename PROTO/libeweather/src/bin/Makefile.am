
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@E_CFLAGS@

bin_PROGRAMS = eweather_test

eweather_test_SOURCES = eweather_test.c

eweather_test_LDADD = @E_LIBS@ $(top_builddir)/src/lib/libeweather.la

