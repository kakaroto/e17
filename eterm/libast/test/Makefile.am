# $Id$

EXTRA_PROGRAMS = libast-test

libast_test_SOURCES = test.c test.h
libast_test_DEPENDENCIES = $(top_builddir)/src/libast.la
libast_test_LDADD = $(top_builddir)/src/libast.la

test: libast-test
	$(top_builddir)/test/libast-test

.PHONY: test
