
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_builddir) \
-I$(top_srcdir)/bin \
-I$(top_builddir)/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\"

AM_CFLAGS = @EXM_CFLAGS@

bin_PROGRAMS = examine examine_test

examine_SOURCES = examine_main.c
examine_LDFLAGS = -Wl,--enable-auto-import

examine_test_SOURCES = examine_test.c
examine_test_LDFLAGS = -Wl,--enable-auto-import
