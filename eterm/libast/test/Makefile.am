# $Id$

bin_PROGRAMS = libast-test

libast_test_SOURCES = test.c
libast_test_DEPENDENCIES = $(top_builddir)/libast.la
libast_test_LDADD = $(top_builddir)/libast.la

test: libast-test
	$(top_builddir)/test/libast-test

.PHONY: test
