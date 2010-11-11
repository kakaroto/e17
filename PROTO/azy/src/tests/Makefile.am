SUBDIRS = unit

BUILT_SOURCES = \
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
  bench.h \
  bench.sh \
  test.zer \
  server.pem \
  zblok/build.sh \
  zblok/client.c \
  zblok/server.c \
  zblok/server.pem \
  zblok/zblok.azy

AM_CFLAGS= \
  $(ECORE_CFLAGS) \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include

AM_LDFLAGS = -static

CLEANFILES = $(BUILT_SOURCES) .sources-ts
MAINTAINERCLEANFILES = Makefile.in

LDADD = \
  $(ECORE_LIBS) \
  $(top_srcdir)/src/lib/libazy.la

check_PROGRAMS = \
  client \
  stress_client \
  server
#  session-client \
#  value-utils-test

client_SOURCES = \
  client.c \
  $(BUILT_SOURCES)

#session_client_SOURCES = \
#  session-client.c \
#  $(BUILT_SOURCES)

stress_client_SOURCES = \
  stress_client.c \
  $(BUILT_SOURCES)

server_SOURCES = \
  server.c \
  $(BUILT_SOURCES)

#value_utils_test_SOURCES = \
#  value-utils-test.c

$(BUILT_SOURCES): .sources-ts

.sources-ts: test.zer $(top_srcdir)/src/bin/azy_parser
	$(top_srcdir)/src/bin/azy_parser test.zer
	touch .sources-ts
