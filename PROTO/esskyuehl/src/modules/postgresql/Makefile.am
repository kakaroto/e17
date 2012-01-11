MAINTAINERCLEANFILES = Makefile.in

moddir = $(libdir)/esskyuehl/$(MODULE_ARCH)
mod_LTLIBRARIES = psql.la

psql_la_SOURCES = \
esql_postgresql_backend.c

psql_la_CFLAGS = \
  @EFL_CFLAGS@ \
  -I$(top_srcdir)/src/lib \
  @POSTGRESQL_CFLAGS@
psql_la_LIBADD = \
  $(top_builddir)/src/lib/libesskyuehl.la \
  @EFL_LIBS@ @POSTGRESQL_LIBS@
psql_la_LDFLAGS = -no-undefined -module -avoid-version
psql_la_LIBTOOLFLAGS = --tag=disable-static

