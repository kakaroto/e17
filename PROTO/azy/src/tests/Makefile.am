SUBDIRS = unit

if HAVE_MYSQL
MYSQL_COMMON_S = T_SQL.h
else
MYSQL_COMMON_S =
endif

COMMON_S = \
  T_Common.h \
  T_Common.c \
  T_Test1.h \
  T_Test2.h

if HAVE_MYSQL
MYSQL_SERVER_S = \
  T_SQL.azy_server.c \
  T_SQL.azy_server_stubs.c \
  T_SQL.azy_server.h \
  T_SQL.azy_server_stubs.h
else
MYSQL_SERVER_S =
endif

SERVER_S = \
  T_Test1.azy_server.c \
  T_Test1.azy_server_stubs.c \
  T_Test1.azy_server.h \
  T_Test1.azy_server_stubs.h \
  T_Test2.azy_server.c \
  T_Test2.azy_server_stubs.c \
  T_Test2.azy_server.h \
  T_Test2.azy_server_stubs.h

if HAVE_MYSQL
MYSQL_CLIENT_S = \
  T_SQL.azy_client.c \
  T_SQL.azy_client.h
else
MYSQL_CLIENT_S = 
endif

CLIENT_S = \
  T_Test1.azy_client.c \
  T_Test1.azy_client.h \
  T_Test2.azy_client.c \
  T_Test2.azy_client.h


BUILT_SOURCES = \
  $(COMMON_S) \
  $(MYSQL_COMMON_S) \
  $(CLIENT_S) \
  $(MYSQL_CLIENT_S) \
  $(SERVER_S) \
  $(MYSQL_SERVER_S)

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

nodist_client_SOURCES = \
  client.c \
  $(COMMON_SOURCES) \
  $(MYSQL_COMMON_S) \
  $(CLIENT_SOURCES) \
  $(MYSQL_CLIENT_S)


nodist_stress_client_SOURCES = \
  stress_client.c \
  $(COMMON_SOURCES) \
  $(MYSQL_COMMON_S) \
  $(CLIENT_SOURCES) \
  $(MYSQL_CLIENT_S)

nodist_server_SOURCES = \
  server.c \
  $(COMMON_SOURCES) \
  $(MYSQL_COMMON_S) \
  $(SERVER_SOURCES) \
  $(MYSQL_SERVER_S)

$(BUILT_SOURCES): .sources-ts

.sources-ts: test.azy $(top_srcdir)/src/bin/azy_parser
	$(top_srcdir)/src/bin/azy_parser test.azy
	touch .sources-ts
