SUBDIRS = mysac
MAINTAINERCLEANFILES = Makefile.in

moddir = $(libdir)/esskyuehl/$(MODULE_ARCH)
mod_LTLIBRARIES = mysql.la

mysql_la_SOURCES = \
esql_mysql_backend.c

mysql_la_CFLAGS = \
  @EFL_CFLAGS@ \
  @MYSQL_CFLAGS@ \
  -I$(top_srcdir)/src/lib \
  -Imysac
mysql_la_LIBADD = \
  $(top_builddir)/src/lib/libesskyuehl.la \
  @EFL_LIBS@ @MYSQL_LIBS@ \
  mysac/libmysac.la
mysql_la_LDFLAGS = -no-undefined -module -avoid-version
mysql_la_LIBTOOLFLAGS = --tag=disable-static

