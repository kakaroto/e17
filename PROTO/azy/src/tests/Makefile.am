SUBDIRS = unit identi.ca rss

TYPES_S = \
  $(top_builddir)/src/tests/type1_Common.c \
  $(top_builddir)/src/tests/type1_Common.h \
  $(top_builddir)/src/tests/type1_Common_Azy.c \
  $(top_builddir)/src/tests/type1_Common_Azy.h \
  $(top_builddir)/src/tests/type2_Common.c \
  $(top_builddir)/src/tests/type2_Common.h \
  $(top_builddir)/src/tests/type2_Common_Azy.c \
  $(top_builddir)/src/tests/type2_Common_Azy.h

COMMON_S = \
  $(top_builddir)/src/tests/T_Common.h \
  $(top_builddir)/src/tests/T_Common.c \
  $(top_builddir)/src/tests/T_Common_Azy.c \
  $(top_builddir)/src/tests/T_Common_Azy.h

if HAVE_MYSQL
MYSQL_SERVER_S = \
  $(top_builddir)/src/tests/T_SQL.azy_server.c \
  $(top_builddir)/src/tests/T_SQL.azy_server.h
else
MYSQL_SERVER_S =
endif

SERVER_S = \
  $(MYSQL_SERVER_S) \
  $(top_builddir)/src/tests/T_Test1.azy_server.c \
  $(top_builddir)/src/tests/T_Test1.azy_server.h \
  $(top_builddir)/src/tests/T_Test2.azy_server.c \
  $(top_builddir)/src/tests/T_Test2.azy_server.h

if HAVE_MYSQL
MYSQL_CLIENT_S = \
  $(top_builddir)/src/tests/T_SQL.azy_client.c \
  $(top_builddir)/src/tests/T_SQL.azy_client.h
else
MYSQL_CLIENT_S = 
endif

CLIENT_S = \
  $(MYSQL_CLIENT_S) \
  $(top_builddir)/src/tests/T_Test1.azy_client.c \
  $(top_builddir)/src/tests/T_Test1.azy_client.h \
  $(top_builddir)/src/tests/T_Test2.azy_client.c \
  $(top_builddir)/src/tests/T_Test2.azy_client.h


BUILT_SOURCES = .sources

EXTRA_DIST = \
  client.c \
  server.c \
  stress_client.c \
  test.azy \
  type1.azy \
  type2.azy \
  server.pem

AM_CFLAGS= \
  $(AZY_CFLAGS) \
  $(MYSQL_CFLAGS) \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include
AM_CXXFLAGS = $(AM_CFLAGS)

CLEANFILES = \
  $(top_builddir)/src/tests/T_SQL.azy_server.c \
  $(top_builddir)/src/tests/T_SQL.azy_server.h \
  $(top_builddir)/src/tests/T_SQL.azy_client.c \
  $(top_builddir)/src/tests/T_SQL.azy_client.h \
  $(TYPES_S) \
  $(CLIENT_S) \
  $(COMMON_S) \
  $(SERVER_S)

MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = \
  client \
  stress_client \
  server

noinst_LTLIBRARIES = libclient.la libserver.la

nodist_libclient_la_SOURCES = \
  $(COMMON_S) \
  $(CLIENT_S)

nodist_libserver_la_SOURCES = \
  $(COMMON_S) \
  $(SERVER_S)

libclient_la_LIBADD = \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la

libserver_la_LIBADD = \
  $(AZY_LIBS) \
  $(MYSQL_LIBS) \
  $(top_builddir)/src/lib/libazy.la

nodist_client_SOURCES = \
  client.c
client_LDADD = \
  $(MYSQL_LIBS) \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la \
  libclient.la

nodist_stress_client_SOURCES = \
  stress_client.c
stress_client_LDADD = \
  $(MYSQL_LIBS) \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la \
  libclient.la

nodist_server_SOURCES = \
  server.c

server_LDADD = \
  $(MYSQL_LIBS) \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la \
  libserver.la

.sources: test.azy $(top_builddir)/src/bin/azy_parser
	$(top_builddir)/src/bin/azy_parser -H -p -o $(top_builddir)/src/tests $(top_srcdir)/src/tests/test.azy
	$(top_builddir)/src/bin/azy_parser -H -p -o $(top_builddir)/src/tests $(top_srcdir)/src/tests/type1.azy
	$(top_builddir)/src/bin/azy_parser -H -p -o $(top_builddir)/src/tests $(top_srcdir)/src/tests/type2.azy
