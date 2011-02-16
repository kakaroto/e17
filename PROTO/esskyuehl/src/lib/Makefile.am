SUBDIRS =
SQL_S =
SQL_L =

if MYSQL
SUBDIRS += mysql
SQL_S += mysql/esql_mysql_backend.c
SQL_L += mysql/mysac/libmysac.la
endif

if POSTGRESQL
SQL_S += postgresql/esql_postgresql_backend.c
endif

pkginclude_HEADERS = Esskyuehl.h
pkgincludedir = @includedir@/esskyuehl-@VMAJ@

AM_CFLAGS = \
  @EFL_CFLAGS@ \
  @MYSQL_CFLAGS@ \
  @POSTGRESQL_CFLAGS@ \
  -I@top_srcdir@


lib_LTLIBRARIES = libesskyuehl.la

libesskyuehl_la_LIBADD = \
  @EFL_LIBS@ \
  @MYSQL_LIBS@ \
  @POSTGRESQL_LIBS@ \
  $(SQL_L)

libesskyuehl_la_LDFLAGS = -version-info @version_info@ @release_info@ -no-undefined --as-needed

libesskyuehl_la_SOURCES = \
esql.c \
esql_connect.c \
esql_events.c \
esql_pool.c \
esql_query.c \
esql_res.c \
$(SQL_S)

MAINTAINERCLEANFILES = Makefile.in
