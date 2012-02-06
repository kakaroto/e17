
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DSYSTEM_CONFIG_DIR=\"$(sysconfdir)\" \
@EKBD_TEST_CFLAGS@

bin_PROGRAMS = ekbd_test

ekbd_test_SOURCES = \
ekbd_test.c

ekbd_test_LDADD = @EKBD_TEST_LIBS@ $(top_builddir)/src/lib/libekbd.la

