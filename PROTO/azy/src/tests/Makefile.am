SUBDIRS = unit

BUILT_SOURCES = \
  T_Common.h \
  T_Common.c \
  TTest1.azy_server.c \
  TTest1.azy_client.c \
  TTest1.azy_server_stubs.c \
  TTest1.azy_server.h \
  TTest1.azy_client.h \
  TTest1.azy_server_stubs.h \
  TTest1.h \
  TTest2.azy_server.c \
  TTest2.azy_client.c \
  TTest2.azy_server_stubs.c \
  TTest2.azy_server.h \
  TTest2.azy_client.h \
  TTest2.azy_server_stubs.h \
  TTest2.h

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

LDADD = \
  $(ECORE_LIBS) \
  $(top_srcdir)/src/lib/libazy.la

check_PROGRAMS = \
  client \
  server
#  tclient \
#  session-client \
#  value-utils-test

client_SOURCES = \
  client.c \
  $(BUILT_SOURCES)

#session_client_SOURCES = \
#  session-client.c \
#  $(BUILT_SOURCES)

#tclient_SOURCES = \
#  tclient.c \
#  $(BUILT_SOURCES)

server_SOURCES = \
  server.c \
  $(BUILT_SOURCES)

#value_utils_test_SOURCES = \
#  value-utils-test.c

$(BUILT_SOURCES): .sources-ts

.sources-ts: test.zer $(top_srcdir)/src/bin/azy_parser
	$(top_srcdir)/src/bin/azy_parser test.zer
	touch .sources-ts
