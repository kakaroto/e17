MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/src

noinst_PROGRAMS = edb_test

edb_test_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_test_SOURCES = edb_test.c
edb_test_LDFLAGS = -static 
edb_test_LDADD = $(top_builddir)/src/libedb.la
