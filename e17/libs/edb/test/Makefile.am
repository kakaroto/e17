INCLUDES = \
        -I$(top_srcdir)/src -I/usr/local/include

noinst_PROGRAMS = edb_test

edb_test_DEPENDENCIES = $(top_srcdir)/src/libedb.la

edb_test_SOURCES = edb_test.c

edb_test_LDFLAGS = -static 

edb_test_LDADD = $(top_builddir)/src/libedb.la -ldb
