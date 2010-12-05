SUBDIRS = unit

MYSQL_S = \
  T_SQL.azy_server.c \
  T_SQL.azy_client.c \
  T_SQL.azy_server_stubs.c \
  T_SQL.azy_server.h \
  T_SQL.azy_client.h \
  T_SQL.azy_server_stubs.h \
  T_SQL.h

BUILT_SOURCES = \
  $(MYSQL_S) \
  T_Common.h \
  T_Common.c \
  T_Test1.azy_server.c \
  T_Test1.azy_client.c \
  T_Test1.azy_server_stubs.c \
  T_Test1.azy_server.h \
  T_Test1.azy_client.h \
  T_Test1.azy_server_stubs.h \
  T_Test1.h \
  T_Test2.azy_server.c \
  T_Test2.azy_client.c \
  T_Test2.azy_server_stubs.c \
  T_Test2.azy_server.h \
  T_Test2.azy_client.h \
  T_Test2.azy_server_stubs.h \
  T_Test2.h


EXTRA_DIST = \
  test.azy \
  server.pem

AM_CFLAGS= \
  $(ECORE_CFLAGS) \
  $(MYSQL_CFLAGS) \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include

AM_LDFLAGS = -static

CLEANFILES = $(BUILT_SOURCES) .sources-ts
MAINTAINERCLEANFILES = Makefile.in

LDADD = \
  $(ECORE_LIBS) \
  $(MYSQL_LIBS) \
  $(top_srcdir)/src/lib/libazy.la

check_PROGRAMS = \
  client \
  stress_client \
  server

client_SOURCES = \
  client.c \
  $(BUILT_SOURCES)

stress_client_SOURCES = \
  stress_client.c \
  $(BUILT_SOURCES)

server_SOURCES = \
  server.c \
  $(BUILT_SOURCES)

$(BUILT_SOURCES): .sources-ts

.sources-ts: test.azy $(top_srcdir)/src/bin/azy_parser
	$(top_srcdir)/src/bin/azy_parser test.azy
	touch .sources-ts
