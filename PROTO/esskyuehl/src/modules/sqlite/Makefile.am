MAINTAINERCLEANFILES = Makefile.in

moddir = $(libdir)/esskyuehl/$(MODULE_ARCH)
mod_LTLIBRARIES = sqlite.la

sqlite_la_SOURCES = \
esql_sqlite_backend.c

sqlite_la_CFLAGS = \
  @EFL_CFLAGS@ \
  -I$(top_srcdir)/src/lib \
  @SQLITE3_CFLAGS@
sqlite_la_LIBADD = \
  $(top_builddir)/src/lib/libesskyuehl.la \
  @EFL_LIBS@ @SQLITE3_LIBS@
sqlite_la_LDFLAGS = -no-undefined -module -avoid-version
sqlite_la_LIBTOOLFLAGS = --tag=disable-static

