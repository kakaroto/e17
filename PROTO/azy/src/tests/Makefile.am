SUBDIRS = unit

if HAVE_MYSQL
MYSQL_COMMON_S = \
  $(top_builddir)/src/tests/T_SQL.c \
  $(top_builddir)/src/tests/T_SQL.h
else
MYSQL_COMMON_S =
endif

COMMON_S = \
  $(MYSQL_COMMON_S) \
  $(top_builddir)/src/tests/T_Common.h \
  $(top_builddir)/src/tests/T_Common.c \
  $(top_builddir)/src/tests/T_Test1.h \
  $(top_builddir)/src/tests/T_Test1.c \
  $(top_builddir)/src/tests/T_Test2.c \
  $(top_builddir)/src/tests/T_Test2.h

if HAVE_MYSQL
MYSQL_SERVER_S = \
  $(top_builddir)/src/tests/T_SQL.azy_server.c \
  $(top_builddir)/src/tests/T_SQL.azy_server_stubs.c \
  $(top_builddir)/src/tests/T_SQL.azy_server.h \
  $(top_builddir)/src/tests/T_SQL.azy_server_stubs.h
else
MYSQL_SERVER_S =
endif

SERVER_S = \
  $(MYSQL_SERVER_S) \
  $(top_builddir)/src/tests/T_Test1.azy_server.c \
  $(top_builddir)/src/tests/T_Test1.azy_server_stubs.c \
  $(top_builddir)/src/tests/T_Test1.azy_server.h \
  $(top_builddir)/src/tests/T_Test1.azy_server_stubs.h \
  $(top_builddir)/src/tests/T_Test2.azy_server.c \
  $(top_builddir)/src/tests/T_Test2.azy_server_stubs.c \
  $(top_builddir)/src/tests/T_Test2.azy_server.h \
  $(top_builddir)/src/tests/T_Test2.azy_server_stubs.h

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


BUILT_SOURCES = \
  $(COMMON_S) \
  $(CLIENT_S) \
  $(SERVER_S)

EXTRA_DIST = \
  client.c \
  server.c \
  stress_client.c \
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
  $(top_builddir)/src/lib/libazy.la

check_PROGRAMS = \
  client \
  stress_client \
  server

nodist_client_SOURCES = \
  client.c \
  $(COMMON_S) \
  $(CLIENT_S)


nodist_stress_client_SOURCES = \
  stress_client.c \
  $(COMMON_S) \
  $(CLIENT_S)

nodist_server_SOURCES = \
  server.c \
  $(COMMON_S) \
  $(SERVER_S)

$(BUILT_SOURCES): .sources-ts

.sources-ts: test.azy $(top_builddir)/src/bin/azy_parser
	$(top_builddir)/src/bin/azy_parser $(top_srcdir)/src/tests/test.azy -o $(top_builddir)/src/tests
	touch .sources-ts
