# $Id$

EXTRA_PROGRAMS = libast-test libast-perf

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/include/$(PACKAGE)

libast_test_SOURCES = test.c test.h
libast_test_DEPENDENCIES = $(top_builddir)/src/libast.la
libast_test_LDADD = $(top_builddir)/src/libast.la

libast_perf_SOURCES = perf.c perf.h
libast_perf_DEPENDENCIES = $(top_builddir)/src/libast.la
libast_perf_LDADD = $(top_builddir)/src/libast.la

test: libast-test
	$(top_builddir)/test/libast-test

perf: libast-perf
	$(top_builddir)/test/libast-perf

.PHONY: test
