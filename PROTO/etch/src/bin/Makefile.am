MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
@ETCH_CFLAGS@

bin_PROGRAMS = etch_test

etch_test_DEPENDENCIES = $(top_builddir)/src/lib/libetch.la

etch_test_SOURCES = \
etch_test.c

etch_test_LDADD = \
$(top_builddir)/src/lib/libetch.la \
@ETCH_LIBS@