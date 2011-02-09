MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
  @MYSQL_CFLAGS@ \
  @EFL_CFLAGS@ \
  -I@top_builddir@ \
  -I@top_builddir@/src/lib

LDADD = \
  @MYSQL_LIBS@ \
  @EFL_LIBS@ \
  @top_builddir@/src/lib/libesskyuehl.la

check_PROGRAMS = basic_query

basic_query_SOURCES = basic_query.c
